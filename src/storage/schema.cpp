#include "schema.hpp"

namespace chuspapulse::storage {

void inicializarEsquema(SQLite::Database& db) {

    db.exec(
        "CREATE TABLE IF NOT EXISTS snapshots ("
        "    timestamp        INTEGER NOT NULL,"
        "    cpu_usage        REAL    NOT NULL DEFAULT 0,"
        "    cpu_cores        INTEGER NOT NULL DEFAULT 0,"
        "    memory_total     INTEGER NOT NULL DEFAULT 0,"
        "    memory_used      INTEGER NOT NULL DEFAULT 0,"
        "    memory_available INTEGER NOT NULL DEFAULT 0,"
        "    disk_total       INTEGER NOT NULL DEFAULT 0,"
        "    disk_used        INTEGER NOT NULL DEFAULT 0,"
        "    disk_free        INTEGER NOT NULL DEFAULT 0,"
        "    network_rx_bytes INTEGER NOT NULL DEFAULT 0,"
        "    network_tx_bytes INTEGER NOT NULL DEFAULT 0"
        ");"
    );

    db.exec(
        "CREATE INDEX IF NOT EXISTS idx_snapshots_timestamp "
        "ON snapshots(timestamp);"
    );

    db.exec(
        "CREATE TABLE IF NOT EXISTS alerts ("
        "    id        INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    timestamp INTEGER NOT NULL,"
        "    metrica   TEXT    NOT NULL,"
        "    umbral    REAL    NOT NULL,"
        "    valor     REAL    NOT NULL,"
        "    mensaje   TEXT    NOT NULL"
        ");"
    );

    db.exec(
        "CREATE INDEX IF NOT EXISTS idx_alerts_timestamp "
        "ON alerts(timestamp);"
    );
}

} // namespace chuspapulse::storage
