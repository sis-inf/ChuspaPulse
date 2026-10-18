#pragma once

#include <httplib.h>
#include "../storage/storage.hpp"
#include "../formatters/formatter_json.hpp"

namespace pulso::http {

/**
 * @brief Registra el handler del endpoint GET /metrics.
 *
 * El handler retorna el snapshot más reciente almacenado en Storage,
 * serializado como JSON usando FormatterJSON.
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
  const pulso::storage::Storage& storage);

} // namespace pulso::http
