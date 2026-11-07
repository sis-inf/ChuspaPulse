#include "handler_metrics.hpp"
#include "utils/logging/logger.hpp"

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace chuspapulse::http {

void HandleMetrics(
  httplib::Server& servidor,
  SystemMonitor&   system_monitor)
{
  servidor.Get("/metrics", [&system_monitor](
    const httplib::Request& req,
    httplib::Response&      res)
  {
    auto& logger = chuspapulse::utils::logging::Logger::instancia();

    auto metrics = system_monitor.getMetrics();

    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&now_time_t), "%Y-%m-%dT%H:%M:%SZ");
    metrics["timestamp"] = ss.str();

    int status_code = 0;

    if (!metrics.empty()) {
      chuspapulse::formatters::FormatterJSON formatter;

      std::string body         = formatter.formatear(metrics);
      std::string content_type = formatter.contentType();

      res.set_content(body, content_type);
      res.status = 200;
      status_code = 200;
    } else {
      res.set_content(
        R"({ "error": "no data available" })",
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

} // namespace chuspapulse::http