#include "http/handler_prometheus.hpp"
#include "formatters/formatter_prometheus.hpp"

#include <httplib.h>

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
        [&storage](const httplib::Request& /*req*/, httplib::Response& res)
        {
            auto body = renderizarUltimoSnapshotPrometheus(storage);

            // Prometheus tolera respuestas vacías; devolvemos 200 con body vacío.
            res.status = 200;
            res.set_content(body.value_or(""), kContentTypePrometheus);
        });
}

} // namespace chuspapulse::http
