#pragma once

#include <cstdint>

namespace chuspapulse::collectors {

/**
 * @brief Información de procesos obtenida desde /proc.
 */
struct ProcInfo {
    uint32_t running{}; ///< Procesos actualmente en ejecución
    uint32_t blocked{}; ///< Procesos bloqueados esperando I/O
    uint32_t total{};   ///< Total de procesos del sistema
};

/**
 * @brief Obtiene estadísticas de procesos desde /proc/stat y /proc/loadavg.
 *
 * Lee:
 * - /proc/stat:
 *   - procs_running
 *   - procs_blocked
 *
 * - /proc/loadavg:
 *   - campo activos/total (4to campo)
 *
 * @return ProcInfo estructura con métricas de procesos.
 *
 * @throws std::runtime_error si no se pueden abrir o parsear
 *         los archivos requeridos.
 */
ProcInfo getProcInfo();

} // namespace chuspapulse::collectors
