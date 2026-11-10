#pragma once

#include <httplib.h>
#include "../core/SystemMonitor.hpp"
#include "../formatters/formatter_json.hpp"

namespace chuspapulse::http {

/**
 * @brief Registra el handler del endpoint GET /metrics en el servidor HTTP.
 *
 * El handler retorna el snapshot más reciente obtenido directamente desde
 * SystemMonitor, serializado como JSON usando FormatterJSON.
 *
 * - Si hay datos: responde con HTTP 200 y el body en JSON.
 * - Si no hay datos: responde con HTTP 503 y un JSON de error.
 *
 * Cada request se registra en el log con nivel INFO incluyendo
 * método, ruta y código de respuesta.
 *
 * @param servidor       Referencia al servidor httplib donde se registra el handler.
 * @param system_monitor Referencia al monitor del sistema para obtener las métricas actuales.
 */
void HandleMetrics(
  httplib::Server& servidor,
  SystemMonitor&   system_monitor);

} // namespace chuspapulse::http
