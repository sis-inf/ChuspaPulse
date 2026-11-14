#include "handler_metrics.hpp"
#include "handler_prometheus.hpp"
#include "utils/logging/logger.hpp"

#include <exception>
#include <string>

namespace chuspapulse::http {

void HandleMetrics(
  httplib::Server&                     servidor,
  const chuspapulse::storage::Storage& storage)
{
  servidor.Get("/metrics", [&storage](
    const httplib::Request& req,
    httplib::Response&      res)
  {
    // /metrics no recibe parametros: la query string se ignora a proposito.
    auto& logger = chuspapulse::utils::logging::Logger::instancia();

    try {
      auto body = renderizarUltimoSnapshotPrometheus(storage);

      if (body) {
        res.set_content(*body, kContentTypePrometheus);
        res.status = 200;
      } else {
        res.set_content(
          R"({"error":"no data available"})",
          "application/json"
        );
        res.status = 503;
      }
    } catch (const std::exception& e) {
      logger.error(std::string("Error al servir /metrics: ") + e.what());
      res.set_content(
        R"({"error":"error interno"})",
        "application/json"
      );
      res.status = 500;
    } catch (...) {
      logger.error("Error desconocido al servir /metrics");
      res.set_content(
        R"({"error":"error interno"})",
        "application/json"
      );
      res.status = 500;
    }

    logger.info(
      req.method + " " + req.path +
      " -> " + std::to_string(res.status)
    );
  });
}

} // namespace chuspapulse::http
