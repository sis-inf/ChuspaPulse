#pragma once

#include <string>
#include <vector>
#include "core/snapshot.hpp"

namespace chuspapulse::formatters {

/**
 * @brief Interfaz abstracta para los formateadores de métricas.
 *
 * Define el contrato que deben cumplir todos los formateadores
 * del sistema ChuspaPulse. Cada implementación serializa un Snapshot
 * en un formato específico (JSON, Prometheus, etc.).
 *
 * Contrato común:
 * - Un snapshot sin métricas, o al que le faltan algunas, no debe
 *   provocar errores: cada implementación define su salida en ese caso.
 * - El escape de caracteres especiales es responsabilidad de cada
 *   implementación y sigue las reglas de su formato.
 *
 * Comportamiento de las implementaciones actuales:
 * - CSV: las columnas fijas sin métrica quedan en 0.00 y las métricas
 *   desconocidas se ignoran. Solo escribe números, por lo que no
 *   requiere escape.
 * - JSON: usa nlohmann::json, que escapa los textos según las reglas
 *   de JSON. Sin métricas, el arreglo "metricas" queda vacío.
 * - Texto: usa valores en cero para las métricas que faltan y ignora
 *   las desconocidas. Solo imprime números y la fecha.
 * - Prometheus: normaliza los nombres de las métricas y escapa el
 *   texto de las líneas HELP. Sin métricas devuelve una cadena vacía.
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
     *
     * Un formateador puede mantener estado interno entre llamadas
     * (por ejemplo, el CSV emite la cabecera solo en la primera llamada).
     *
     * @param snapshot El snapshot a serializar.
     * @return Cadena con el snapshot en el formato correspondiente.
     */
    virtual std::string formatear(const chuspapulse::core::Snapshot& snapshot) const = 0;

    /**
     * @brief Serializa una lista de snapshots (historial).
     *
     * Con un historial vacío cada implementación devuelve su salida
     * mínima: el CSV, solo la cabecera; el JSON, "total" en 0 con un
     * arreglo vacío; el texto y Prometheus, una cadena vacía.
     * Prometheus serializa solo el último snapshot del historial.
     *
     * @param snapshots Vector de snapshots a serializar.
     * @return Cadena con el historial en el formato correspondiente.
     */
    virtual std::string formatearHistorial(
        const std::vector<chuspapulse::core::Snapshot>& snapshots) const = 0;
};

} // namespace chuspapulse::formatters
