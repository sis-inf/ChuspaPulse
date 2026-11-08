#pragma once

#include <string>
#include <vector>

#include "iformatter.hpp"

namespace chuspapulse::formatters {

class FormatterPrometheus : public IFormatter {
public:
    std::string formato() const override;

    std::string contentType() const override;

    std::string formatear(
        const chuspapulse::core::Snapshot& snapshot) const override;

    std::string formatearHistorial(
        const std::vector<chuspapulse::core::Snapshot>& snapshots) const override;

    /**
     * @brief Convierte un nombre interno (sin etiquetas) en un nombre
     *        Prometheus válido: [a-zA-Z_:][a-zA-Z0-9_:]*.
     *
     * Antepone "chuspapulse_" y reemplaza cada carácter que no sea letra
     * ASCII, dígito ni '_' por '_' (también ':', reservado para reglas de
     * grabación). Es determinista: el mismo nombre siempre da el mismo
     * resultado. Ejemplo: "disk.sda-1 used" -> "chuspapulse_disk_sda_1_used".
     *
     * Si en un snapshot dos nombres distintos dan el mismo resultado,
     * formatear() les agrega un sufijo con un hash del nombre original
     * para no fusionarlos en silencio.
     */
    static std::string normalizarNombre(const std::string& nombre);
};

} // namespace chuspapulse::formatters
