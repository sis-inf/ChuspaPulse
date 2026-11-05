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
 * Responde 400 si algún parámetro es inválido.
 */
void registrarAlertsHistory(
    httplib::Server& servidor,
    const chuspapulse::alertas::GestorAlertas& gestor);

} // namespace chuspapulse::http
