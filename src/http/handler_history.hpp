#pragma once

#include <httplib.h>

#include "../storage/storage.hpp"
#include "../formatters/iformatter.hpp"

namespace chuspapulse::http {

/**
 * @brief Registra el endpoint GET /metrics/history.
 *
 * Parámetros opcionales (enteros):
 * - `desde`: timestamp Unix inicial, mayor o igual a 0 (por defecto, hace 1 hora).
 * - `hasta`: timestamp Unix final; 0 sin límite, si no mayor o igual a `desde`
 *   (por defecto 0). Si solo se envía `hasta`, se compara contra el `desde`
 *   por defecto, por lo que un valor menor responde 400.
 * - `limite`: cantidad máxima de snapshots, de 1 a 10000 (por defecto 1000).
 *
 * Cualquier otro parámetro de la query se ignora sin error.
 *
 * Respuestas:
 * - 200 con el historial en el formato del formatter (usa su Content-Type).
 * - 400 `{"error":"parametro invalido: ..."}` (application/json) si algún
 *   parámetro es inválido.
 * - 500 `{"error":"error interno"}` (application/json) si falla la consulta
 *   o la serialización.
 */
void registrarHistory(
    httplib::Server& servidor,
    const chuspapulse::storage::Storage& storage,
    const chuspapulse::formatters::IFormatter& formatter);

} // namespace chuspapulse::http
