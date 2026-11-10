#include "handler_health.hpp"
#include "core/version.hpp"

#include <chrono>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

namespace chuspapulse::http {

std::string handleHealth(std::chrono::steady_clock::time_point start_time) {

    // Calcular uptime en segundos
    auto ahora   = std::chrono::steady_clock::now();
    auto uptime  = std::chrono::duration_cast<std::chrono::seconds>(
                       ahora - start_time).count();

    // Obtener hostname
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        std::strncpy(hostname, "unknown", sizeof(hostname) - 1);
        hostname[sizeof(hostname) - 1] = '\0';
    }

    // Construir JSON
    std::ostringstream json;
    json << "{"
         << "\"status\":\"ok\","
         << "\"uptime_seconds\":" << uptime << ","
         << "\"version\":\"" << chuspapulse::APP_VERSION << "\","
         << "\"hostname\":\"" << hostname << "\""
         << "}";

    return json.str();
}

ResultadoReadiness handleReadiness(
    const std::vector<ComprobacionReadiness>& comprobaciones) {

    bool listo = true;

    std::ostringstream componentes;
    componentes << "{";

    for (std::size_t i = 0; i < comprobaciones.size(); ++i) {
        const auto& [nombre, comprobar] = comprobaciones[i];

        bool disponible = false;
        try {
            disponible = comprobar && comprobar();
        } catch (const std::exception&) {
            disponible = false;
        }

        listo = listo && disponible;

        if (i > 0) {
            componentes << ",";
        }
        componentes << "\"" << nombre << "\":"
                    << (disponible ? "true" : "false");
    }

    componentes << "}";

    std::ostringstream json;
    json << "{"
         << "\"status\":\"" << (listo ? "ready" : "not_ready") << "\","
         << "\"componentes\":" << componentes.str()
         << "}";

    return {listo ? 200 : 503, json.str()};
}

} // namespace chuspapulse::http
