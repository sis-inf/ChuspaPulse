#include "ram_usage.hpp"

#include "../error_recoleccion.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include <chrono>

namespace chuspapulse::collectors::memory {

namespace {

[[noreturn]] void registrarError(const std::string& mensaje) {
    std::cerr << mensaje << '\n';
    throw ErrorRecoleccion(mensaje);
}

} // namespace

RamInfo getRamUsage(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        registrarError(
            "ram_usage: No se pudo abrir el archivo " + path + ": "
            "verifique permisos y disponibilidad del sistema"
        );
    }

    std::string line;
    long long memTotal     = 0;
    long long memAvailable = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        long long value = 0;

        if (!(iss >> key)) {
            continue;
        }

        if (key != "MemTotal:" && key != "MemAvailable:") {
            continue;
        }

        if (!(iss >> value)) {
            registrarError(
                "ram_usage: Valor invalido para " + key + " en " + path
            );
        }

        if (key == "MemTotal:") {
            memTotal = value;
        }

        if (key == "MemAvailable:") {
            memAvailable = value;
        }
    }

    if (file.bad() || !file.eof()) {
        registrarError(
            "ram_usage: Error durante la lectura del archivo " + path
        );
    }

    if (memTotal <= 0 || memAvailable <= 0) {
        registrarError(
            "ram_usage: Claves MemTotal o MemAvailable ausentes "
            "o con valores invalidos en " + path
        );
    }

    if (memAvailable > memTotal) {
        registrarError(
            "ram_usage: MemAvailable supera MemTotal en " + path
        );
    }

    const long long maxKb =
        std::numeric_limits<long long>::max() / 1024;

    if (memTotal > maxKb || memAvailable > maxKb) {
        registrarError(
            "ram_usage: Desbordamiento al convertir memoria a bytes en " + path
        );
    }

    // Convertir de kB a bytes
    memTotal     *= 1024;
    memAvailable *= 1024;

    // Calcular memoria usada: MemTotal - MemAvailable
    long long memUsed = memTotal - memAvailable;

    return {
        static_cast<uint64_t>(memTotal),
        static_cast<uint64_t>(memUsed),
        static_cast<uint64_t>(memAvailable)
    };
}

std::string CollectorMemory::nombre() const {
    return "memory";
}

std::vector<chuspapulse::core::Metrica> CollectorMemory::recolectar() {

    RamInfo ram = getRamUsage();

    auto timestamp = static_cast<std::int64_t>(
        std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
        )
    );

    return {
        {
            "memory.used",
            static_cast<double>(ram.used),
            "bytes",
            timestamp
        }
    };
}

} // namespace chuspapulse::collectors::memory
