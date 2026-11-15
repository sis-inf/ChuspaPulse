# Guía de Migración de Esquema de Base de Datos

## Descripción

ChuspaPulse usa SQLite como base de datos local (`pulso.db`). Al actualizar a una nueva versión, el esquema puede haber cambiado — por ejemplo, se agregaron nuevas columnas o tablas. Este documento explica cómo funciona la inicialización del esquema y cómo migrar bases de datos existentes de versiones anteriores.

---

## Cómo funciona `inicializarEsquema`

La función `inicializarEsquema` crea las tablas e índices necesarios usando `CREATE TABLE IF NOT EXISTS` y `CREATE INDEX IF NOT EXISTS`.

Esto significa que es **idempotente**: se puede ejecutar múltiples veces sobre la misma base de datos sin producir errores ni borrar datos existentes.

**Sin embargo, esta idempotencia tiene una limitación importante:**

> `inicializarEsquema` puede crear tablas nuevas, pero **no agrega columnas nuevas** a tablas que ya existen en bases de datos de versiones anteriores.

### Ejemplo

Si en una versión anterior `snapshots` no tenía la columna `hostname`, y actualizas ChuspaPulse a una versión que sí la incluye, `inicializarEsquema` **no** agregará esa columna a tu base de datos existente. La tabla ya existe, por lo que el `CREATE TABLE IF NOT EXISTS` no hace nada.

---

## Esquema actual

```sql
CREATE TABLE IF NOT EXISTS snapshots (
    timestamp INTEGER NOT NULL,
    nombre    TEXT    NOT NULL,
    valor     REAL    NOT NULL,
    unidad    TEXT    NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_snapshots_timestamp
ON snapshots(timestamp);
```

### Columnas agregadas en versiones recientes

| Columna | Tabla | Versión | Descripción |
|---------|-------|---------|-------------|
| `hostname` | `snapshots` | Sprint 2 | Nombre del host que generó el snapshot |

---

## Haz un backup antes de actualizar

> **Antes de actualizar ChuspaPulse a una nueva versión, siempre haz un backup de tu base de datos.**

Puedes exportar tus datos con el subcomando `storage export`:

```bash
pulso storage export --output backup.csv
```

Esto genera un archivo CSV con todos los snapshots almacenados que puedes conservar como respaldo.

---

## Cómo migrar una base de datos existente

Si ya tienes una `pulso.db` de una versión anterior y quieres agregar columnas nuevas sin perder datos, ejecuta las migraciones manualmente con cualquier cliente SQLite.

### Agregar la columna `hostname` a `snapshots`

```sql
ALTER TABLE snapshots ADD COLUMN hostname TEXT;
```

### Poblar `hostname` retroactivamente

Si quieres llenar la columna `hostname` en los registros existentes con el nombre del host actual:

```sql
UPDATE snapshots SET hostname = 'nombre-de-tu-host' WHERE hostname IS NULL;
```

O usando la función de SQLite:

```bash
sqlite3 pulso.db "UPDATE snapshots SET hostname = (SELECT value FROM pragma_compile_options WHERE value LIKE 'host%') WHERE hostname IS NULL;"
```

Si no necesitas datos históricos con hostname, puedes dejarlo como `NULL` — ChuspaPulse lo completará automáticamente en los nuevos snapshots.

---

## Proceso recomendado al actualizar ChuspaPulse

1. **Exportar datos actuales:**
```bash
   pulso storage export --output backup-$(date +%Y%m%d).csv
```

2. **Hacer copia de la base de datos:**
```bash
   cp pulso.db pulso.db.bak
```

3. **Actualizar el binario de ChuspaPulse.**

4. **Ejecutar las migraciones manuales** si el esquema cambió (ver sección anterior).

5. **Verificar que ChuspaPulse inicia correctamente:**
```bash
   pulso --once
```

---

## Notas

- Si prefieres empezar desde cero, puedes borrar `pulso.db` y ChuspaPulse creará una base de datos nueva al iniciar.
- Las migraciones automáticas están planificadas para versiones futuras del proyecto.