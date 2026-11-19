#pragma once
#include <string>
#include <vector>
#include "../../collectors/icollector.hpp"
#include "../../collectors/error_recoleccion.hpp"
#include "../../core/types.hpp"

namespace chuspapulse::platform::linux_platform {

/**
 * @brief Collector que mide el uso de memoria RAM desde /proc/meminfo.
 *
 * Lee los campos MemTotal y MemAvailable del pseudo-archivo /proc/meminfo
 * del kernel de Linux. El espacio usado se calcula como:
 *
 *   memory.used_bytes = MemTotal - MemAvailable
 *
 * Esta fórmula es la misma que usa `free(1)` en su columna "used" y refleja
 * la memoria realmente consumida por procesos y caché no recuperable, a
 * diferencia de MemTotal - MemFree, que excluye caché y buffers reutilizables.
 *
 * Las métricas devueltas son:
 *   - memory.total_bytes    : total de RAM física (MemTotal × 1024)
 *   - memory.used_bytes     : RAM en uso efectivo (MemTotal - MemAvailable) × 1024
 *   - memory.available_bytes: RAM disponible para nuevas asignaciones (MemAvailable × 1024)
 *
 * La invariante memory.used_bytes <= memory.total_bytes se mantiene siempre
 * porque MemAvailable <= MemTotal en cualquier sistema Linux.
 */
class CollectorMemoryLinux : public chuspapulse::collectors::ICollector {
public:
    /// @brief Retorna el nombre identificador de este collector.
    /// @return "memory"
    std::string nombre() const override;

    /// @brief Recolecta las métricas de uso de memoria RAM del sistema.
    /// @return Vector con memory.total_bytes, memory.used_bytes y
    ///         memory.available_bytes, todos en bytes.
    /// @throws chuspapulse::collectors::ErrorRecoleccion si /proc/meminfo no puede
    ///         abrirse o si los campos requeridos no están presentes.
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

} // namespace chuspapulse::platform::linux_platform