#include "bateria_collector.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace chuspapulse::collectors::bateria {

namespace {

std::string recortarEspacios(const std::string& texto) {
    const auto inicio = std::find_if_not(texto.begin(), texto.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });
    if (inicio == texto.end()) {
        return "";
    }
    const auto fin = std::find_if_not(texto.rbegin(), texto.rend(), [](unsigned char c) {
        return std::isspace(c) != 0;
    }).base();
    return std::string(inicio, fin);
}

BateriaInfo estadoNoDisponible() {
    return BateriaInfo{
        /* porcentaje */ 0,
        /* estado     */ "",
        /* disponible */ false
    };
}

} 

BateriaInfo getBateriaInfo(const std::string& base_path) {
    try {
        if (base_path.empty()) {
            std::cerr << "[bateria_collector] Error: la ruta base de suministro de energia esta vacia.\n";
            return estadoNoDisponible();
        }

        std::error_code ec;
        const std::filesystem::path ruta_base(base_path);
        if (!std::filesystem::exists(ruta_base, ec) || ec) {
            std::clog << "[bateria_collector] Fuente de bateria no disponible en '" << base_path
                      << "'" << (ec ? " (" + ec.message() + ")" : "") << ".\n";
            return estadoNoDisponible();
        }

        if (!std::filesystem::is_directory(ruta_base, ec) || ec) {
            std::cerr << "[bateria_collector] Error: la ruta '" << base_path
                      << "' no es un directorio valido.\n";
            return estadoNoDisponible();
        }

        const std::string capacity_path = (ruta_base / "capacity").string();
        const std::string status_path   = (ruta_base / "status").string();

        std::ifstream capacity_file(capacity_path);
        if (!capacity_file.is_open()) {
            std::cerr << "[bateria_collector] No se pudo abrir el archivo de capacidad: '"
                      << capacity_path << "'.\n";
            return estadoNoDisponible();
        }

        std::ifstream status_file(status_path);
        if (!status_file.is_open()) {
            std::cerr << "[bateria_collector] No se pudo abrir el archivo de estado: '"
                      << status_path << "'.\n";
            return estadoNoDisponible();
        }

        int porcentaje = 0;
        if (!(capacity_file >> porcentaje)) {
            std::cerr << "[bateria_collector] Error de lectura o formato invalido en '"
                      << capacity_path << "'.\n";
            return estadoNoDisponible();
        }

        if (porcentaje < 0 || porcentaje > 100) {
            std::cerr << "[bateria_collector] Valor de capacidad fuera de rango (" << porcentaje
                      << "%) en '" << capacity_path << "'.\n";
            return estadoNoDisponible();
        }

        std::string estado_crudo;
        if (!std::getline(status_file, estado_crudo)) {
            std::cerr << "[bateria_collector] Error al leer el estado de carga desde '"
                      << status_path << "'.\n";
            return estadoNoDisponible();
        }

        const std::string estado = recortarEspacios(estado_crudo);
        if (estado.empty()) {
            std::cerr << "[bateria_collector] El archivo de estado '" << status_path
                      << "' esta vacio o contiene solo espacios.\n";
            return estadoNoDisponible();
        }

        return BateriaInfo{
            /* porcentaje */ porcentaje,
            /* estado     */ estado,
            /* disponible */ true
        };
    } catch (const std::exception& ex) {
        std::cerr << "[bateria_collector] Excepcion controlada al leer informacion de bateria en '"
                  << base_path << "': " << ex.what() << "\n";
        return estadoNoDisponible();
    } catch (...) {
        std::cerr << "[bateria_collector] Error desconocido controlado al leer informacion de bateria en '"
                  << base_path << "'.\n";
        return estadoNoDisponible();
    }
}

std::string CollectorBateria::nombre() const {
    return "bateria";
}

std::vector<chuspapulse::core::Metrica> CollectorBateria::recolectar() {
    try {
        BateriaInfo info = getBateriaInfo();

        // Sin bateria presente o fuente inaccesible: vector vacio, sin abortar el proceso.
        if (!info.disponible) {
            return {};
        }

        const auto timestamp = static_cast<std::int64_t>(
            std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now()
            )
        );

        std::vector<chuspapulse::core::Metrica> metricas;
        metricas.reserve(2);

        metricas.push_back({
            "bateria.porcentaje",
            static_cast<double>(info.porcentaje),
            "porcentaje",
            timestamp
        });

        // "Charging" -> 1.0, cualquier otro estado (Discharging, Full,
        // Not charging, Unknown) -> 0.0.
        const double cargando = (info.estado == "Charging") ? 1.0 : 0.0;

        metricas.push_back({
            "bateria.cargando",
            cargando,
            "booleano",
            timestamp
        });

        return metricas;
    } catch (const std::exception& ex) {
        std::cerr << "[bateria_collector] Error inesperado durante la recoleccion de metricas: "
                  << ex.what() << "\n";
        return {};
    } catch (...) {
        std::cerr << "[bateria_collector] Error desconocido durante la recoleccion de metricas.\n";
        return {};
    }
}

} 
