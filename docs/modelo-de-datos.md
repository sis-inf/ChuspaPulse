# Modelo de datos

## Resumen

ChuspaPulse almacena la información recolectada en una base de datos SQLite. El esquema está definido en `storage/schema.cpp` (función `inicializarEsquema`) y contiene dos tablas: `snapshots`, con las métricas recolectadas, y `alerts`, con el historial de alertas disparadas.

La inicialización es idempotente: todas las sentencias usan `CREATE ... IF NOT EXISTS`, por lo que puede ejecutarse cada vez que se abre la base sin perder datos. La clase `Storage` la invoca automáticamente en su constructor, después de activar el modo WAL (`PRAGMA journal_mode=WAL`).

---

# Diagrama del modelo de datos

```text
+----------------------------+        +----------------------------+
|         snapshots          |        |           alerts           |
+----------------------------+        +----------------------------+
| timestamp                  |        | id (PK)                    |
| cpu_usage                  |        | timestamp                  |
| cpu_cores                  |        | metrica                    |
| memory_total               |        | umbral                     |
| memory_used                |        | valor                      |
| memory_available           |        | mensaje                    |
| disk_total                 |        +----------------------------+
| disk_used                  |
| disk_free                  |
| network_rx_bytes           |
| network_tx_bytes           |
+----------------------------+
```

---

# Tabla `snapshots`

Guarda una fila por snapshot, es decir, por cada ciclo de muestreo. Cada métrica soportada tiene su propia columna.

## Columnas

| Columna | Tipo | Métrica | Unidad | Descripción |
|---------|------|---------|--------|-------------|
| timestamp | INTEGER NOT NULL | — | segundos (Unix) | Momento de captura del snapshot |
| cpu_usage | REAL | `cpu.usage` | percent | Uso de CPU |
| cpu_cores | INTEGER | `cpu.cores` | cores | Cantidad de núcleos |
| memory_total | INTEGER | `memory.total` | bytes | Memoria total |
| memory_used | INTEGER | `memory.used` | bytes | Memoria en uso |
| memory_available | INTEGER | `memory.available` | bytes | Memoria disponible |
| disk_total | INTEGER | `disk.total` | bytes | Capacidad total del disco |
| disk_used | INTEGER | `disk.used` | bytes | Espacio usado del disco |
| disk_free | INTEGER | `disk.free` | bytes | Espacio libre del disco |
| network_rx_bytes | INTEGER | `network.rx_bytes` | bytes | Bytes recibidos por red |
| network_tx_bytes | INTEGER | `network.tx_bytes` | bytes | Bytes enviados por red |

Todas las columnas de métricas son `NOT NULL DEFAULT 0`. Si un snapshot no incluye una métrica, `Storage::save` guarda `0` en su columna.

## Correspondencia con `core::Snapshot`

- Al guardar (`Storage::save`), cada `Metrica` del snapshot se busca por su `nombre` y su `valor` se escribe en la columna correspondiente.
- Al recuperar (`Storage::last` y `Storage::history`), cada fila se convierte en un `Snapshot` con las diez métricas de la tabla anterior, con su `nombre`, `unidad` y el `timestamp` de la fila.

## Índices

- `idx_snapshots_timestamp` sobre `timestamp`, usado en las consultas por rango de `history()` y en el orden de `last()`.

## Retención

`Storage` conserva como máximo `maxEntries` filas (3600 por defecto, configurable con `setMaxEntries`). Al superar el límite se eliminan las filas más antiguas por `timestamp` (política FIFO).

---

# Tabla `alerts`

Registra las alertas disparadas cuando una métrica supera el umbral configurado. La usa `alertas::GestorAlertas`.

## Columnas

| Columna | Tipo | Descripción |
|---------|------|-------------|
| id | INTEGER PRIMARY KEY AUTOINCREMENT | Identificador de la alerta |
| timestamp | INTEGER NOT NULL | Momento de la alerta (Unix, segundos) |
| metrica | TEXT NOT NULL | Métrica que disparó la alerta |
| umbral | REAL NOT NULL | Umbral configurado |
| valor | REAL NOT NULL | Valor observado |
| mensaje | TEXT NOT NULL | Mensaje descriptivo de la alerta |

## Índices

- `idx_alerts_timestamp` sobre `timestamp`.

---

# Referencias

- `storage/schema.cpp`
- `storage/storage.cpp`
- `storage/storage.hpp`
