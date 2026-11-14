#pragma once

#include <httplib.h>
#include <optional>
#include <string>
#include "storage/storage.hpp"

namespace chuspapulse::http {

/// Content-Type del formato de exposición de texto de Prometheus.
inline const std::string kContentTypePrometheus = "text/plain; version=0.0.4";

/**
 * @brief Serializa el último snapshot guardado en formato Prometheus.
 *
 * Los nombres se exponen con el prefijo `chuspapulse_` y los puntos se
 * reemplazan por guiones bajos (por ejemplo, `cpu.usage` se expone como
 * `chuspapulse_cpu_usage`). Cada métrica declara `# HELP` y `# TYPE gauge`.
 *
 * @param storage Almacenamiento del que se lee el último snapshot.
 * @return Texto en formato Prometheus, o std::nullopt si todavía no hay
 *         un snapshot con métricas.
 */
std::optional<std::string> renderizarUltimoSnapshotPrometheus(
    const chuspapulse::storage::Storage& storage);

/**
 * @brief Registra el handler de /metrics/prometheus en el servidor httplib.
 *
 * Contrato de la respuesta:
 *  - 200: texto en formato Prometheus (`kContentTypePrometheus`); body vacío
 *    si todavía no hay un snapshot con métricas.
 *  - 400: JSON `{"error":"..."}` si la petición trae query strings, porque
 *    este endpoint no acepta parámetros.
 *  - 500: JSON `{"error":"..."}` ante cualquier fallo interno.
 *
 * @param servidor Servidor httplib donde se registra la ruta.
 * @param storage  Almacenamiento del que se lee el último snapshot.
 */
void registrarPrometheus(
    httplib::Server& servidor,
    const chuspapulse::storage::Storage& storage);

} // namespace chuspapulse::http
