#include "uptime_collector.h"
#include <cmath>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

UptimeInfo estadoUptimeControlado() {
    UptimeInfo info{};
    info.total_seconds = 0;
    info.days = 0;
    info.hours = 0;
    info.minutes = 0;
    info.seconds = 0;
    info.formatted = "0d 0h 0m 0s";
    return info;
}

}

UptimeInfo getUptime() {
    try {    // Abrir el archivo virtual del sistema operativo Linux
        std::ifstream file("/proc/uptime");

        // Criterio de aceptación: Lanzar std::runtime_error si no existe o no se puede abrir
        if (!file.is_open()) {
            throw std::runtime_error("Error: No se pudo abrir o no existe el archivo /proc/uptime");
        }

        double uptime_seconds = 0.0;

        // Leer el primer campo de /proc/uptime, que representa el uptime en segundos
        if (!(file >> uptime_seconds)) {
            std::cerr << "[uptime_collector] Fallo de lectura: el contenido o formato de /proc/uptime es invalido o vacio. "
                      << "Devolviendo estado controlado.\n";
            return estadoUptimeControlado();
        }
        if (!std::isfinite(uptime_seconds) || uptime_seconds < 0.0) {
            std::cerr << "[uptime_collector] Error de validacion: valor de tiempo activo fuera de rango ("
                      << uptime_seconds << ") en /proc/uptime. Devolviendo estado controlado.\n";
            return estadoUptimeControlado();
        }

        const uint64_t total_seconds = static_cast<uint64_t>(uptime_seconds);

        UptimeInfo info{};
        info.total_seconds = total_seconds;

        info.days = static_cast<uint32_t>(total_seconds / 86400);

        uint64_t remaining = total_seconds % 86400;

        info.hours = static_cast<uint32_t>(remaining / 3600);
        remaining %= 3600;

        info.minutes = static_cast<uint32_t>(remaining / 60);
        info.seconds = static_cast<uint32_t>(remaining % 60);

        std::ostringstream formatted;
        formatted << info.days << "d "
                  << info.hours << "h "
                  << info.minutes << "m "
                  << info.seconds << "s";

        info.formatted = formatted.str();

        return info;
    } catch (const std::exception& ex) {
        std::cerr << "[uptime_collector] Excepcion controlada al recolectar uptime: " << ex.what()
                  << ". Devolviendo estado controlado.\n";
        return estadoUptimeControlado();
    } catch (...) {
        std::cerr << "[uptime_collector] Error desconocido controlado al recolectar uptime. "
                  << "Devolviendo estado controlado.\n";
        return estadoUptimeControlado();
    }
}
