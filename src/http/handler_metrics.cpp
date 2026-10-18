#include "handler_metrics.hpp"
#include "utils/logging/logger.hpp"

#include <string>

namespace pulso::http {

void HandleMetrics(
  httplib::Server& servidor,
  const pulso::storage::Storage& storage)
{
  servidor.Get("/metrics", [&storage](
    const httplib::Request& req,
    httplib::Response& res)
  {
    auto& logger = pulso::utils::logging::Logger::instancia();

    auto snapshot = storage.last();

    int status_code = 0;

    if (snapshot) {
      pulso::formatters::FormatterJSON formatter;

      std::string body = formatter.formatear(*snapshot);

      res.set_content(body, formatter.contentType());
      res.status = 200;
      status_code = 200;
    } else {
      res.set_content(
        R"({"error":"no data available"})",
        "application/json"
      );
      res.status = 503;
      status_code = 503;
    }

    logger.info(
      req.method + " " + req.path +
      " -> " + std::to_string(status_code)
    );
  });
}

} // namespace pulso::http