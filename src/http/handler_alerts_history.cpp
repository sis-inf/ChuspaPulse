#include "handler_alerts_history.hpp"

#include <cstddef>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace chuspapulse::http {

namespace {

/// Convierte el texto completo a entero; falla si sobra algún carácter.
bool leerEntero(const std::string& texto, std::int64_t& valor)
{
    try {
        std::size_t pos = 0;
        valor = std::stoll(texto, &pos);
        return pos == texto.size();
    }
    catch (const std::exception&) {
        return false;
    }
}

void responderError(httplib::Response& res, int status, const std::string& mensaje)
{
    res.status = status;
    res.set_content(nlohmann::json{{"error", mensaje}}.dump(), "application/json");
}

} // namespace

void registrarAlertsHistory(
    httplib::Server& servidor,
    const chuspapulse::alertas::GestorAlertas& gestor)
{
    servidor.Get("/alerts/history",
        [&gestor](const httplib::Request& req, httplib::Response& res)
    {
        std::int64_t limite = 100;
        std::int64_t offset = 0;

        if (req.has_param("limite") &&
            (!leerEntero(req.get_param_value("limite"), limite) ||
             limite < 1 || limite > 1000)) {
            responderError(res, 400,
                "parametro invalido: limite debe ser un entero entre 1 y 1000");
            return;
        }
        if (req.has_param("offset") &&
            (!leerEntero(req.get_param_value("offset"), offset) || offset < 0)) {
            responderError(res, 400,
                "parametro invalido: offset debe ser un entero mayor o igual a 0");
            return;
        }

        try {
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
            responderError(res, 500, "error interno");
        }
    });
}

} // namespace chuspapulse::http
