#include "handler_alerts_history.hpp"

#include <cstddef>
#include <nlohmann/json.hpp>
#include <string>

namespace chuspapulse::http {

void registrarAlertsHistory(
    httplib::Server& servidor,
    const chuspapulse::alertas::GestorAlertas& gestor)
{
    servidor.Get("/alerts/history",
        [&gestor](const httplib::Request& req, httplib::Response& res)
    {
        try {
            int limite = 100;
            int offset = 0;

            if (req.has_param("limite")) {
                limite = std::stoi(req.get_param_value("limite"));
            }
            if (req.has_param("offset")) {
                offset = std::stoi(req.get_param_value("offset"));
            }

            if (limite < 1 || limite > 1000 || offset < 0) {
                res.status = 400;
                res.set_content(
                    R"({"error":"parametros invalidos"})",
                    "application/json");
                return;
            }

            auto json = nlohmann::json::array();
            for (const auto& r : gestor.obtenerHistorial(
                     static_cast<std::size_t>(limite),
                     static_cast<std::size_t>(offset))) {
                json.push_back({
                    {"id", r.id},
                    {"timestamp", r.timestamp},
                    {"metrica", r.metrica},
                    {"umbral", r.umbral},
                    {"valor", r.valor},
                    {"mensaje", r.mensaje}
                });
            }

            res.status = 200;
            res.set_content(json.dump(), "application/json");
        }
        catch (const std::exception&) {
            res.status = 400;
            res.set_content(
                R"({"error":"parametros invalidos"})",
                "application/json");
        }
    });
}

} // namespace chuspapulse::http