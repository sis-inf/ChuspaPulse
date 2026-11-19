#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../icollector.hpp"
#include "core/types.hpp"

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
 * @param path Ruta al archivo de información de memoria (por defecto "/proc/meminfo").
 *             Permite inyectar una ruta alternativa en tests sin modificar el código de producción.
 * @return RamInfo con los valores de memoria total, usada y disponible.
 */
RamInfo getRamUsage(const std::string& path = "/proc/meminfo");

/**
 * @brief Collector de métricas de memoria RAM.
 */
class CollectorMemory : public chuspapulse::collectors::ICollector {
public:
    std::string nombre() const override;

    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

} // namespace chuspapulse::collectors::memory
