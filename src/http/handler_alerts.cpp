#include "http/handler_alerts.hpp"

#include <chrono>
#include <ctime>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "alertas/gestor_alertas.hpp"
#include "core/MetricSnapshot.hpp"

namespace chuspapulse::http {

namespace {

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

std::string serializarAlertas(const std::vector<AlertaActiva>& alertas) {
    nlohmann::json lista = nlohmann::json::array();

    for (const auto& alerta : alertas) {
        lista.push_back({
            {"id", alerta.id},
            {"message", alerta.mensaje},
            {"timestamp", alerta.timestamp},
        });
    }

    const nlohmann::json respuesta = {{"alerts", lista}};

    return respuesta.dump(2, ' ', false,
                          nlohmann::json::error_handler_t::replace);
}

std::string handleAlerts(const chuspapulse::alertas::GestorAlertas& gestor,
                         const chuspapulse::MetricSnapshot& snap) {
    const std::string timestamp = formatearTimestamp(snap.timestamp);

    std::vector<AlertaActiva> activas;
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

        activas.push_back({id, alerta.mensaje(), timestamp});
    }

    return serializarAlertas(activas);
}

} // namespace chuspapulse::http