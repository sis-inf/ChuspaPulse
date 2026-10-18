#include "storage.hpp"
#include "schema.hpp"
#include <ctime>
#include <map>
#include <fstream>
#include <stdexcept>

namespace pulso::storage {

namespace {

void pruneSnapshots(SQLite::Database& db, std::size_t maxEntries) {
    SQLite::Statement countQuery(db, "SELECT COUNT(*) FROM snapshots;");
    countQuery.executeStep();

    const auto total = static_cast<std::size_t>(
        countQuery.getColumn(0).getInt64()
    );

    if (total <= maxEntries) {
        return;
    }

    const auto toDelete = static_cast<std::int64_t>(total - maxEntries);

    SQLite::Statement deleteQuery(
        db,
        "DELETE FROM snapshots "
        "WHERE rowid IN ("
        "SELECT rowid FROM snapshots "
        "ORDER BY timestamp ASC, rowid ASC "
        "LIMIT ?"
        ");"
    );

    deleteQuery.bind(1, toDelete);
    deleteQuery.exec();
}

} // namespace

Storage::Storage(const std::string& dbPath)
    : db_(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
    // Activar modo WAL para mejorar rendimiento en escrituras concurrentes
    db_.exec("PRAGMA journal_mode=WAL;");

    // Inicializar esquema (idempotente)
    pulso::storage::inicializarEsquema(db_);
}

void Storage::setMaxEntries(std::size_t max) {
    maxEntries_ = max;
    pruneSnapshots(db_, maxEntries_);
}

void Storage::exportToCSV(const std::string& ruta_archivo) const {
    std::ofstream archivo(ruta_archivo);

    if (!archivo.is_open()) {
        throw std::runtime_error(
            "No se pudo crear el archivo CSV: " + ruta_archivo
        );
    }

    // Cabecera CSV
    archivo
        << "timestamp,"
        << "cpu_usage,"
        << "cpu_cores,"
        << "memory_total,"
        << "memory_used,"
        << "memory_available,"
        << "disk_total,"
        << "disk_used,"
        << "disk_free,"
        << "network_rx_bytes,"
        << "network_tx_bytes"
        << '\n';

    SQLite::Statement query(
        db_,
        "SELECT "
        "timestamp, "
        "cpu_usage, cpu_cores, "
        "memory_total, memory_used, memory_available, "
        "disk_total, disk_used, disk_free, "
        "network_rx_bytes, network_tx_bytes "
        "FROM snapshots "
        "ORDER BY timestamp ASC;"
    );

    while (query.executeStep()) {
        archivo
            << query.getColumn(0).getInt64() << ","
            << query.getColumn(1).getDouble() << ","
            << query.getColumn(2).getInt() << ","
            << query.getColumn(3).getInt64() << ","
            << query.getColumn(4).getInt64() << ","
            << query.getColumn(5).getInt64() << ","
            << query.getColumn(6).getInt64() << ","
            << query.getColumn(7).getInt64() << ","
            << query.getColumn(8).getInt64() << ","
            << query.getColumn(9).getInt64() << ","
            << query.getColumn(10).getInt64()
            << '\n';
    }
}

void Storage::save(const pulso::core::Snapshot& snapshot) {
    SQLite::Statement query(
        db_,
        "INSERT INTO snapshots (timestamp, nombre, valor, unidad) "
        "VALUES (?, ?, ?, ?);"
    );

    for (const auto& m : snapshot.metricas) {
        query.bind(1, snapshot.timestamp);
        query.bind(2, m.nombre);
        query.bind(3, m.valor);
        query.bind(4, m.unidad);
        query.exec();
        query.reset();
    }

    pruneSnapshots(db_, maxEntries_);
}

// Convierte las filas de una consulta en un Snapshot
static pulso::core::Snapshot rowToSnapshot(SQLite::Statement& query) {
    pulso::core::Snapshot snapshot;
    snapshot.timestamp = query.getColumn(0).getInt64();

    pulso::core::Metrica m;
    m.nombre = query.getColumn(1).getString();
    m.valor = query.getColumn(2).getDouble();
    m.unidad = query.getColumn(3).getString();
    m.timestamp = snapshot.timestamp;

    snapshot.metricas.push_back(m);
    return snapshot;
}

std::optional<pulso::core::Snapshot> Storage::last() const {
    SQLite::Statement query(
        db_,
        "SELECT timestamp, nombre, valor, unidad "
        "FROM snapshots WHERE timestamp = (SELECT MAX(timestamp) FROM snapshots) "
        "ORDER BY rowid ASC;"
    );

    if (!query.executeStep()) {
        return std::nullopt;
    }

    auto snapshot = rowToSnapshot(query);

    while (query.executeStep()) {
        if (query.getColumn(0).getInt64() != snapshot.timestamp) {
            break;
        }

        pulso::core::Metrica m;
        m.nombre = query.getColumn(1).getString();
        m.valor = query.getColumn(2).getDouble();
        m.unidad = query.getColumn(3).getString();
        m.timestamp = snapshot.timestamp;
        snapshot.metricas.push_back(m);
    }

    return snapshot;
}

std::vector<pulso::core::Snapshot> Storage::history(
    std::int64_t from,
    std::int64_t until,
    std::size_t limit) const
{
    std::string sql =
        "SELECT timestamp, nombre, valor, unidad "
        "FROM snapshots WHERE timestamp >= ?";

    if (until != 0) {
        sql += " AND timestamp <= ?";
    }

    sql += " ORDER BY timestamp ASC LIMIT ?;";

    SQLite::Statement query(db_, sql);

    int bindIndex = 1;
    query.bind(bindIndex++, from);

    if (until != 0) {
        query.bind(bindIndex++, until);
    }

    query.bind(bindIndex, static_cast<std::int64_t>(limit));

    std::vector<pulso::core::Snapshot> results;

    while (query.executeStep()) {
        auto fila = rowToSnapshot(query);
        auto it = results.empty() ? results.end() : results.end();

        if (!results.empty() &&
            results.back().timestamp == fila.timestamp) {
            results.back().metricas.push_back(fila.metricas.front());
        } else {
            results.push_back(fila);
        }
    }

    return results;
}

std::size_t Storage::total() const {
    SQLite::Statement query(db_, "SELECT COUNT(*) FROM snapshots;");
    query.executeStep();
    return static_cast<std::size_t>(query.getColumn(0).getInt64());
}

pulso::core::Snapshot Storage::getPromedio(uint32_t ventana_segundos) const {
    // Calcular el timestamp de inicio de la ventana
    std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));
    std::int64_t desde = ahora - static_cast<std::int64_t>(ventana_segundos);

    // Obtener snapshots dentro de la ventana
    auto snapshots = history(desde, ahora);

    // Si no hay snapshots en la ventana, retornar snapshot vacío
    if (snapshots.empty()) {
        pulso::core::Snapshot vacio;
        vacio.timestamp = ahora;
        for (const auto& nombre : {
            "cpu.usage", "cpu.cores",
            "memory.total", "memory.used", "memory.available",
            "disk.total", "disk.used", "disk.free",
            "network.rx_bytes", "network.tx_bytes"}) {
            pulso::core::Metrica m;
            m.nombre      = nombre;
            m.valor     = 0.0;
            m.unidad      = "";
            m.timestamp = ahora;
            vacio.metricas.push_back(m);
        }
        return vacio;
    }

    // Acumular valores por nombre de métrica
    std::map<std::string, double> acumulado;
    std::map<std::string, std::string> unidades;

    for (const auto& snap : snapshots) {
        for (const auto& m : snap.metricas) {
            acumulado[m.nombre] += m.valor;
            unidades[m.nombre]   = m.unidad;
        }
    }

    // Calcular promedio
    pulso::core::Snapshot promedio;
    promedio.timestamp = ahora;

    for (auto& [nombre, total] : acumulado) {
        pulso::core::Metrica m;
        m.nombre      = nombre;
        m.valor     = total / static_cast<double>(snapshots.size());
        m.unidad      = unidades[nombre];
        m.timestamp = ahora;
        promedio.metricas.push_back(m);
    }

    return promedio;
}

} // namespace pulso::storage