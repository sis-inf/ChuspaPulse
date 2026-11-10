#pragma once

#include <string>
#include <vector>

#include "iformatter.hpp"
#include "core/types.hpp"

namespace chuspapulse::formatters {

/**
 * @brief Formateador de texto plano legible para humanos.
 *
 * Genera una línea de texto con las métricas principales del sistema:
 * timestamp | CPU% | RAM usada/total GB | Disco usado/total GB | Red rx/tx KB/s
 */
class FormatterText : public IFormatter {
public:
    /**
     * @brief Retorna el identificador del formato.
     */
    std::string formato() const override;

    /**
     * @brief Retorna el content type HTTP.
     */
    std::string contentType() const override;

    /**
     * @brief Serializa un snapshot individual en texto plano.
     */
    std::string formatear(
        const chuspapulse::core::Snapshot& snapshot
    ) const override;

    /**
     * @brief Serializa un historial de snapshots en texto plano.
     */
    std::string formatearHistorial(
        const std::vector<chuspapulse::core::Snapshot>& snapshots
    ) const override;
};

} // namespace chuspapulse::formatters
