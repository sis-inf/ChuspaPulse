#pragma once

#include <string>
#include <vector>

#include "iformatter.hpp"
#include "core/types.hpp"

namespace chuspapulse::formatters {

/**
 * @brief Formateador JSON para snapshots de métricas.
 *
 * La serialización se hace con nlohmann::json y se emite en formato compacto
 * (sin indentación) y codificación UTF-8, conforme a RFC 8259. Los caracteres
 * especiales de los textos (comillas dobles, barras invertidas y caracteres de
 * control) se escapan automáticamente. nlohmann::json ordena las claves de cada
 * objeto alfabéticamente, y ese es el orden en que aparecen en la salida.
 */
class FormatterJSON : public IFormatter {
public:
    /**
     * @brief Retorna el identificador del formato.
     * @return La cadena "json".
     */
    std::string formato() const override;

    /**
     * @brief Retorna el content type HTTP.
     * @return La cadena "application/json".
     */
    std::string contentType() const override;

    /**
     * @brief Serializa un snapshot individual.
     *
     * Produce un objeto con los campos "metricas" y "timestamp". "metricas" es
     * un arreglo cuyos elementos tienen los campos "nombre", "timestamp",
     * "unidad" y "valor". Solo se serializan las métricas presentes en el
     * snapshot: si contiene únicamente algunas, se emiten esas y no se generan
     * valores para las que faltan. Un snapshot sin métricas produce
     * {"metricas":[],"timestamp":<timestamp>}, que es un JSON válido. Si el
     * valor de una métrica no es finito (NaN o infinito) se serializa como
     * null, porque JSON no admite esos valores.
     *
     * @param snapshot Snapshot a serializar.
     * @return Cadena JSON con el snapshot.
     * @throws nlohmann::json::type_error si algún texto (nombre o unidad)
     *         contiene UTF-8 inválido.
     */
    std::string formatear(
        const chuspapulse::core::Snapshot& snapshot
    ) const override;

    /**
     * @brief Serializa un historial de snapshots.
     *
     * Produce un objeto con los campos "snapshots" y "total" (cantidad de
     * snapshots). Cada elemento de "snapshots" tiene la misma estructura que
     * el resultado de formatear(), con las mismas reglas para métricas
     * parciales o ausentes. Un historial vacío produce
     * {"snapshots":[],"total":0}.
     *
     * @param snapshots Snapshots a serializar, en el orden recibido.
     * @return Cadena JSON con el historial.
     * @throws nlohmann::json::type_error si algún texto (nombre o unidad)
     *         contiene UTF-8 inválido.
     */
    std::string formatearHistorial(
        const std::vector<chuspapulse::core::Snapshot>& snapshots
    ) const override;
};

}
