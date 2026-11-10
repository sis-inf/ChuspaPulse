#pragma once

#include <chrono>
#include <string>

namespace chuspapulse::http {

/**
 * @brief Genera una respuesta JSON para el endpoint GET /health.
 *
 * Retorna un JSON con:
 * - status: siempre "ok"
 * - uptime_seconds: segundos transcurridos desde start_time
 * - version: versión hardcodeada "0.1.0"
 * - hostname: nombre real de la máquina
 *
 * @param start_time Punto en el tiempo cuando el agente inició.
 * @return std::string JSON con la información de health-check.
 */
std::string handleHealth(std::chrono::steady_clock::time_point start_time);

} // namespace chuspapulse::http
