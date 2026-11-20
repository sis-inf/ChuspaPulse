#pragma once

#include <httplib.h>

#include "../alertas/gestor_alertas.hpp"

namespace chuspapulse::http {

/**
 * @brief Registra el endpoint GET /alerts/history.
 *
 * Devuelve el historial de alertas persistido, ordenado por timestamp
 * descendente, con paginación básica mediante los parámetros opcionales
 * `limite` (1 a 1000, por defecto 100) y `offset` (0 o más, por defecto 0).
 * Cualquier otro parámetro o query string se rechaza.
 *
 * Todas las respuestas usan `application/json`:
 * - 200 con el arreglo JSON de alertas.
 * - 400 `{"error":"parametro invalido: ..."}` si `limite` u `offset` no son
 *   enteros o están fuera de rango.
 * - 400 `{"error":"parametro no soportado: ..."}` si la petición incluye un
 *   parámetro distinto de `limite` y `offset`.
 * - 500 `{"error":"error interno"}` si falla la obtención o la serialización.
 */
void registrarAlertsHistory(
    httplib::Server& servidor,
    const chuspapulse::alertas::GestorAlertas& gestor);

} // namespace chuspapulse::http
