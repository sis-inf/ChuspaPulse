#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace chuspapulse::core {

/**
 * @brief Representa una metrica individual del sistema con sus metadatos
 *
 * Una metrica encapsula un valor de medicion puntual, junto con el nombre
 * que lo identifica, la unidad en que se expresa y el momento en que fue tomado
 */
struct Metrica {
    /** @brief Nombre identificador de la metrica. Ej: "cpu.usage", "ram.used" */
    std::string nombre;

    /** @brief Valor numerico de la medicion */
    double valor;

    /** @brief Unidad en que se expresa el valor. Ej: "porcentaje", "bytes" */
    std::string unidad;

    /** @brief Momento en que se tomo la medición, expresado como Unix timestamp en segundos. */
    std::int64_t timestamp;
};

/**
 * @brief Representa un conjunto de métricas capturadas en un instante determinado.
 *
 * Un Snapshot agrupa todas las Metrica recolectadas durante un mismo ciclo de muestreo,
 * permitiendo tener una vision completa del estado del sistema en un momento puntual.
 */
struct Snapshot {
    /** @brief Momento en que se realizo el muestreo, expresado como Unix timestamp en segundos. */
    std::int64_t timestamp;

    /** @brief Coleccion de métricas individuales capturadas en este snapshot. */
    std::vector<Metrica> metricas;
};
} // namespace chuspapulse::core
