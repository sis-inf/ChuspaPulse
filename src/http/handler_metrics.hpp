#pragma once

#include <httplib.h>
#include "../storage/storage.hpp"

namespace chuspapulse::http {

/**
 * @brief Registra el handler del endpoint GET /metrics en el servidor HTTP.
 *
 * El handler expone el snapshot más reciente guardado en Storage por el
 * sampler, en el formato de exposición de texto de Prometheus
 * (`text/plain; version=0.0.4`), con el mismo contenido que
 * /metrics/prometheus.
 *
 * - Si hay datos: responde con HTTP 200 y las métricas.
 * - Si todavía no hay datos: responde con HTTP 503 y un JSON de error.
 * - Si falla la lectura del almacenamiento: responde con HTTP 500.
 *
 * Cada request se registra en el log con nivel INFO incluyendo
 * método, ruta y código de respuesta.
 *
 * @param servidor Referencia al servidor httplib donde se registra el handler.
 * @param storage  Almacenamiento del que se leen las métricas actuales.
 */
void HandleMetrics(
  httplib::Server&                     servidor,
  const chuspapulse::storage::Storage& storage);

} // namespace chuspapulse::http
