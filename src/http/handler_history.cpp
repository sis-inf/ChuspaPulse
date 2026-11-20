#include "handler_history.hpp"

#include <charconv>
#include <cstdint>
#include <ctime>
#include <nlohmann/json.hpp>
#include <string>

namespace chuspapulse::http {

namespace {

bool leerEntero(const std::string& texto, std::int64_t& valor)
{
    if (texto.empty()) {
        return false;
    }

    auto resultado = std::from_chars(
        texto.data(),
        texto.data() + texto.size(),
        valor
    );

    return resultado.ec == std::errc{} &&
           resultado.ptr == texto.data() + texto.size();
}

void responderError(httplib::Response& res, int status, const std::string& mensaje)
{
    res.status = status;
    res.set_content(
        nlohmann::json{{"error", mensaje}}.dump(),
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
        [&storage, &formatter](const httplib::Request& req, httplib::Response& res)
    {
        std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

        std::int64_t desde = ahora - 3600;
        std::int64_t hasta = 0;
        std::int64_t limite = 1000;

        if (req.params.count("desde") > 1 ||
            req.params.count("hasta") > 1 ||
            req.params.count("limite") > 1) {
            responderError(res, 400, "parametro repetido");
            return;
        }

        if (req.has_param("desde") &&
            (!leerEntero(req.get_param_value("desde"), desde) || desde < 0)) {
            responderError(res, 400,
                "parametro invalido: desde debe ser un entero mayor o igual a 0");
            return;
        }

        if (req.has_param("hasta") &&
            (!leerEntero(req.get_param_value("hasta"), hasta) ||
             hasta < 0 || (hasta > 0 && hasta < desde))) {
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

            auto body = formatter.formatearHistorial(snapshots);

            res.status = 200;
            res.set_content(body, formatter.contentType());
        }
        catch (const std::exception&) {
            responderError(res, 500, "error interno");
        }
        catch (...) {
            responderError(res, 500, "error interno");
        }
    });
}

} // namespace chuspapulse::http
