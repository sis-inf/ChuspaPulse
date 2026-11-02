#pragma once

#include "iformatter.hpp"
#include <string>
#include <vector>

namespace chuspapulse::formatters {

/**
 * @brief Formateador de métricas en formato CSV.
 *
 * La primera llamada a formatear() emite la línea de cabecera.
 * Las llamadas siguientes emiten filas de datos separadas por comas.
 * Los valores flotantes se presentan con 2 decimales.
 */
class FormatterCSV : public IFormatter {
public:
    /**
     * @brief Retorna el identificador del formato.
     * @return "csv"
     */
    std::string formato() const override;

    /**
     * @brief Retorna el Content-Type HTTP asociado.
     * @return "text/csv"
     */
    std::string contentType() const override;

    /**
     * @brief Serializa un único snapshot en formato CSV.
     * La primera llamada incluye la cabecera.
     * @param snapshot El snapshot a serializar.
     * @return Cadena CSV con cabecera o fila de datos.
     */
    std::string formatear(const chuspapulse::core::Snapshot& snapshot) const override;

    /**
     * @brief Serializa una lista de snapshots en formato CSV.
     * Incluye cabecera en la primera línea.
     * @param snapshots Vector de snapshots a serializar.
     * @return Cadena CSV completa con cabecera y filas de datos.
     */
    std::string formatearHistorial(
        const std::vector<chuspapulse::core::Snapshot>& snapshots) const override;

private:
    mutable bool cabecераEmitida_ = false;

    static std::string cabecera();
    static std::string filaDeSnapshot(const chuspapulse::core::Snapshot& snapshot);
};

} // namespace chuspapulse::formatters
