#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <utility>
#include <vector>

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

/// Comprobación de un componente requerido: nombre y función que indica
/// si está disponible.
using ComprobacionReadiness = std::pair<std::string, std::function<bool()>>;

/// Resultado del endpoint GET /ready.
struct ResultadoReadiness {
    int status;          ///< 200 si todos los componentes están disponibles, 503 si no.
    std::string cuerpo;  ///< JSON con el estado general y el de cada componente.
};

/**
 * @brief Genera la respuesta del endpoint GET /ready.
 *
 * A diferencia de /health, que solo indica que el proceso está vivo,
 * readiness indica si el servicio puede atender solicitudes: evalúa cada
 * componente requerido y responde:
 * - 200 `{"status":"ready","componentes":{...}}` si todos están disponibles.
 * - 503 `{"status":"not_ready","componentes":{...}}` si alguno no lo está.
 *
 * Una comprobación que lanza una excepción cuenta como no disponible.
 *
 * @param comprobaciones Componentes requeridos y su comprobación.
 * @return ResultadoReadiness con el código HTTP y el cuerpo JSON.
 */
ResultadoReadiness handleReadiness(
    const std::vector<ComprobacionReadiness>& comprobaciones);

} // namespace chuspapulse::http
