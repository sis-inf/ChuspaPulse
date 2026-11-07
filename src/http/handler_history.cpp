#include "handler_history.hpp"

#include <cstdint>
#include <ctime>
#include <string>

namespace chuspapulse::http {

void registrarHistory(
    httplib::Server& servidor,
    const chuspapulse::storage::Storage& storage,
    const chuspapulse::formatters::IFormatter& formatter)
{
    servidor.Get("/metrics/history",
        [&](const httplib::Request& req, httplib::Response& res)
    {
        try {

            std::time_t ahora = std::time(nullptr);

            std::int64_t desde = static_cast<std::int64_t>(ahora) - 3600;
            std::int64_t hasta = 0;
            std::size_t limite = 1000; 

            if (req.has_param("desde")) {
                desde = std::stoll(
                    req.get_param_value("desde")
                );
            }

            if (req.has_param("hasta")) {
                hasta = std::stoll(
                    req.get_param_value("hasta")
                );
            }

            if (req.has_param("limite")) {
                limite = std::stoi(
                    req.get_param_value("limite")
                );
            }

            if (desde < 0 || (hasta > 0 && hasta < desde) ||
                limite < 1 || limite > 10000) {

                res.status = 400;

                res.set_content(
                    R"({"error":"parametros invalidos"})",
                    "application/json"
                );

                return;
            }

            auto snapshots = storage.history(
                desde,
                hasta,
                limite
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

            res.status = 400;

            res.set_content(
                R"({"error":"parametros invalidos"})",
                "application/json"
            );
        }
    });
}

} // namespace chuspapulse::http
