#include "http/handler_prometheus.hpp"
#include "formatters/formatter_prometheus.hpp"
#include "utils/logging/logger.hpp"

#include <httplib.h>

#include <exception>
#include <string>

namespace chuspapulse::http {

std::optional<std::string> renderizarUltimoSnapshotPrometheus(
    const chuspapulse::storage::Storage& storage)
{
    auto snapshot = storage.last();

    if (!snapshot || snapshot->metricas.empty()) {
        return std::nullopt;
    }

    chuspapulse::formatters::FormatterPrometheus formatter;
    return formatter.formatear(*snapshot);
}

void registrarPrometheus(
    httplib::Server& servidor,
    const chuspapulse::storage::Storage& storage)
{
    servidor.Get("/metrics/prometheus",
        [&storage](const httplib::Request& req, httplib::Response& res)
        {
            auto& logger = chuspapulse::utils::logging::Logger::instancia();

            try {
                // Este endpoint no acepta query strings: cualquier parametro es un 400.
                if (!req.params.empty()) {
                    res.status = 400;
                    res.set_content(
                        R"({"error":"parametros no soportados"})",
                        "application/json"
                    );
                } else {
                    auto body = renderizarUltimoSnapshotPrometheus(storage);

                    // Prometheus tolera respuestas vacias; devolvemos 200 con body vacio.
                    res.status = 200;
                    res.set_content(body.value_or(""), kContentTypePrometheus);
                }
            } catch (...) {
                logger.error("handler_prometheus: error interno");
                res.status = 500;
                res.set_content(
                    R"({"error":"error interno"})",
                    "application/json"
                );
            }

            logger.info(
                req.method + " " + req.path +
                " -> " + std::to_string(res.status)
            );
        });
}

} // namespace chuspapulse::http
