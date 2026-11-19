#include "handler_history.hpp"

#include <cstdint>
#include <ctime>
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
    res.set_content(
        R"({"error":")" + mensaje + R"("})",
        "application/json"
    );
}

} // namespace

void registrarHistory(
    httplib::Server& servidor,
    const chuspapulse::storage::Storage& storage,
    const chuspapulse::formatters::IFormatter& formatter)
{
    servidor.Get("/metrics/history",
        [&](const httplib::Request& req, httplib::Response& res)
    {
        std::time_t ahora = std::time(nullptr);

        std::int64_t desde = static_cast<std::int64_t>(ahora) - 3600;
        std::int64_t hasta = 0;
        std::int64_t limite = 1000;

        if (req.has_param("desde") &&
            (!leerEntero(req.get_param_value("desde"), desde) || desde < 0)) {
            responderError(res, 400,
                "parametro invalido: desde debe ser un entero mayor o igual a 0");
            return;
        }

        if (req.has_param("hasta") &&
            (!leerEntero(req.get_param_value("hasta"), hasta) || hasta < 0 ||
             (hasta > 0 && hasta < desde))) {
            responderError(res, 400,
                "parametro invalido: hasta debe ser 0 o un entero mayor o igual a desde");
            return;
        }

        if (req.has_param("limite") &&
            (!leerEntero(req.get_param_value("limite"), limite) ||
             limite < 1 || limite > 10000)) {
            responderError(res, 400,
                "parametro invalido: limite debe ser un entero entre 1 y 10000");
            return;
        }

        try {

            auto snapshots = storage.history(
                desde,
                hasta,
                static_cast<std::size_t>(limite)
            );

            auto body = formatter.formatearHistorial(
                snapshots
            );

            res.status = 200;

            res.set_content(
                body,
                formatter.contentType()
            );
        }
        catch (const std::exception&) {

            responderError(res, 500, "error interno");
        }
    });
}

} // namespace chuspapulse::http
