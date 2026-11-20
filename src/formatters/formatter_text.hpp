#pragma once

#include <string>
#include <vector>

#include "iformatter.hpp"
#include "core/types.hpp"

namespace chuspapulse::formatters {

/**
 * @brief Formateador de texto plano legible para humanos.
 *
 * Por cada snapshot genera cinco líneas: el timestamp en ISO 8601 (UTC) y una
 * línea para CPU (porcentaje), RAM (usada/total), Disco (usado/total) y Red
 * (recibido/enviado por segundo). Los tamaños se muestran en unidades legibles
 * según su magnitud.
 *
 * Si el snapshot está vacío o falta una métrica, su valor se muestra en cero y
 * la salida conserva siempre la misma estructura. La salida solo contiene
 * etiquetas fijas y números, nunca texto libre del snapshot, así que no hay
 * caracteres especiales que escapar.
 */
class FormatterText : public IFormatter {
public:
    /**
     * @brief Retorna el identificador del formato ("text").
     */
    std::string formato() const override;

    /**
     * @brief Retorna el content type HTTP ("text/plain").
     */
    std::string contentType() const override;

    /**
     * @brief Serializa un snapshot individual en texto plano.
     *
     * Un snapshot vacío produce la misma estructura con valores en cero. Si el
     * timestamp no es representable, se usa 1970-01-01T00:00:00Z.
     */
    std::string formatear(
        const chuspapulse::core::Snapshot& snapshot
    ) const override;

    /**
     * @brief Serializa un historial de snapshots en texto plano.
     *
     * Concatena la salida de cada snapshot, cada una seguida de un salto de
     * línea. Un historial vacío devuelve una cadena vacía.
     */
    std::string formatearHistorial(
        const std::vector<chuspapulse::core::Snapshot>& snapshots
    ) const override;
};

} // namespace chuspapulse::formatters
