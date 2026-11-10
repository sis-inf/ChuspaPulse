#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <cstddef>
#include <SQLiteCpp/SQLiteCpp.h>
#include "../core/types.hpp"

namespace chuspapulse::storage {

/**
 * @brief Encapsula todas las operaciones de persistencia contra la base de datos SQLite.
 *
 * La clase Storage es responsable de abrir o crear la base de datos,
 * inicializar el esquema y proveer operaciones CRUD sobre los snapshots.
 * El resto del código no debe manipular SQL directamente.
 */
class Storage {
public:
    /**
     * @brief Abre o crea la base de datos en la ruta indicada.
     *
     * Activa el modo WAL para mejorar el rendimiento en escrituras concurrentes.
     * @param dbPath Ruta al archivo de base de datos SQLite.
     * @throws ErrorStorage si la apertura falla.
     */
    explicit Storage(const std::string& dbPath);

    /**
     * @brief Inserta un snapshot en la base de datos.
     * @param snapshot Snapshot a persistir.
     */
    void save(const chuspapulse::core::Snapshot& snapshot);

    /**
     * @brief Configura el número máximo de snapshots retenidos.
     *
     * Cuando se agrega un nuevo snapshot y se supera este límite,
     * se descarta el snapshot más antiguo siguiendo una política FIFO.
     *
     * El límite por defecto es 3600 entradas.
     *
     * @param max Cantidad máxima de snapshots a conservar.
     */
    void setMaxEntries(std::size_t max);

    /**
     * @brief Devuelve el snapshot más reciente almacenado.
     * @return El snapshot más reciente, o std::nullopt si no hay datos.
     */
    std::optional<chuspapulse::core::Snapshot> last() const;

    /**
     * @brief Devuelve snapshots en el rango de timestamps indicado.
     *
     * @param from  Timestamp Unix de inicio del rango (inclusive).
     * @param until Timestamp Unix de fin del rango (inclusive). Si es 0, no se aplica límite superior.
     * @param limit Cantidad máxima de snapshots a retornar. Por defecto 1000.
     * @return Vector de snapshots que cumplen el filtro.
     */
    std::vector<chuspapulse::core::Snapshot> history(
        std::int64_t from,
        std::int64_t until = 0,
        std::size_t  limit = 1000) const;

    /**
     * @brief Cuenta el total de snapshots almacenados en la base de datos.
     * @return Cantidad total de snapshots.
     */
    std::size_t total() const;

    /**
     * @brief Exporta todos los snapshots a un archivo CSV.
     * @param ruta_archivo Ruta del archivo CSV de salida.
     */
    void exportToCSV(const std::string& ruta_archivo) const;

    /**
     * @brief Calcula el promedio de las métricas en una ventana de tiempo.
     *
     * Promedia todos los snapshots cuyo timestamp esté dentro de los últimos
     * ventana_segundos. Si no hay snapshots en la ventana, retorna un snapshot
     * con todos los campos en 0.
     *
     * @param ventana_segundos Tamaño de la ventana en segundos.
     * @return Snapshot con los valores promediados.
     */
    chuspapulse::core::Snapshot getPromedio(uint32_t ventana_segundos) const;

private:
    SQLite::Database db_;
    std::size_t maxEntries_ = 3600;
};

} // namespace chuspapulse::storage
