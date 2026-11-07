#include "http/handler_alerts.hpp"

#include <chrono>
#include <ctime>
#include <sstream>
#include <stdexcept>

#include "alertas/gestor_alertas.hpp"
#include "core/MetricSnapshot.hpp"

namespace chuspapulse::http {

namespace {

std::string escaparJson(const std::string& texto) {
    std::string salida;
    for (char c : texto) {
        if (c == '"' || c == '\\') salida += '\\';
        salida += c;
    }
    return salida;
}

std::string formatearTimestamp(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_utc{};
#ifdef _WIN32
    gmtime_s(&tm_utc, &t);
#else
    gmtime_r(&t, &tm_utc);
#endif
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &tm_utc);
    return buffer;
}

} // namespace

std::string handleAlerts(const chuspapulse::alertas::GestorAlertas& gestor,
                         const chuspapulse::MetricSnapshot& snap) {
    const std::string timestamp = formatearTimestamp(snap.timestamp);

    std::ostringstream json;
    json << "{\n  \"alerts\": [";

    bool primera = true;
    int id = 0;

    for (const auto& alerta : gestor.getAlertas()) {
        ++id;

        bool activa = false;
        try {
            activa = alerta.evaluar(snap);
        } catch (const std::invalid_argument&) {
            continue;  // métrica desconocida: se ignora esa alerta
        }
        if (!activa) continue;

        if (!primera) json << ",";
        primera = false;

        json << "\n    {\n"
             << "      \"id\": " << id << ",\n"
             << "      \"message\": \"" << escaparJson(alerta.mensaje()) << "\",\n"
             << "      \"timestamp\": \"" << timestamp << "\"\n"
             << "    }";
    }

    if (!primera) json << "\n  ";
    json << "]\n}";

    return json.str();
}

} // namespace chuspapulse::http