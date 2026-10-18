#pragma once

#include <string>
#include <vector>
#include "core/types.hpp"

namespace pulso::formatters {

/**
 * @brief Interfaz abstracta para los formateadores de métricas.
 *
 * Define el contrato que deben cumplir todos los formateadores
 * del sistema Pulso. Cada implementación serializa un Snapshot
 * en un formato específico (JSON, Prometheus, etc.).
 */
class IFormatter {
public:
    /**
     * @brief Destructor virtual por defecto.
     */
    virtual ~IFormatter() = default;

    /**
     * @brief Retorna el identificador del formato.
     * @return Ejemplo: "json", "prometheus"
     */
    virtual std::string formato() const = 0;

    /**
     * @brief Retorna el Content-Type HTTP asociado al formato.
     * @return Ejemplo: "application/json", "text/plain"
     */
    virtual std::string contentType() const = 0;

    /**
     * @brief Serializa un único snapshot de métricas.
     * @param snapshot El snapshot a serializar.
     * @return Cadena con el snapshot en el formato correspondiente.
     */
    virtual std::string formatear(const pulso::core::Snapshot& snapshot) const = 0;

    /**
     * @brief Serializa una lista de snapshots (historial).
     * @param snapshots Vector de snapshots a serializar.
     * @return Cadena con el historial en el formato correspondiente.
     */
    virtual std::string formatearHistorial(
        const std::vector<pulso::core::Snapshot>& snapshots) const = 0;
};

} // namespace pulso::formatters