#pragma once

#include <cstdint>

namespace chuspapulse::collectors::memory {

/**
 * @brief Contiene la informacion de uso de memoria RAM del sistema.
 */
struct RamInfo {
    /** @brief Memoria total disponible en el sistema, expresada en bytes. */
    uint64_t total;

    /** @brief Memoria actualmente en uso, expresada en bytes. */
    uint64_t used;

    /** @brief Memoria disponible para ser utilizada, expresada en bytes. */
    uint64_t available;
};

/**
 * @brief Obtiene las metricas actuales de uso de memoria RAM del sistema.
 * @return RamInfo con los valores de memoria total, usada y disponible.
 */
RamInfo getRamUsage();

} // namespace chuspapulse::collectors::memory
