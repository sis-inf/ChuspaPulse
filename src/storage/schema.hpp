#pragma once

#include <SQLiteCpp/Database.h>

namespace chuspapulse::storage {

/**
 * @brief Inicializa el esquema de la base de datos SQLite.
 *
 * Crea la tabla `snapshots`, con una fila por snapshot y una columna por
 * métrica (CPU, memoria, disco y red), y el índice `idx_snapshots_timestamp`
 * para optimizar las consultas por rango de tiempo. También crea la tabla
 * `alerts` para el historial de alertas disparadas y su índice
 * `idx_alerts_timestamp`. La función es idempotente: puede invocarse
 * múltiples veces sobre la misma base de datos sin producir errores.
 *
 * @param db Referencia a la base de datos SQLite ya abierta.
 * @throws SQLite::Exception si la ejecución del SQL falla por un error
 *         interno de la base de datos.
 */
void inicializarEsquema(SQLite::Database& db);

} // namespace chuspapulse::storage
