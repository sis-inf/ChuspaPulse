# Downsampling — Compactacion de Datos Antiguos

## Que es el downsampling

El downsampling es el proceso de reducir la resolucion de datos antiguos reemplazando multiples snapshots individuales por un unico promedio horario. A diferencia de la purgacion (que elimina datos), el downsampling conserva la informacion en forma compactada.

**Importante: el downsampling es una operacion destructiva e irreversible sobre la resolucion original.** Una vez compactados, los datos individuales de cada muestreo no pueden recuperarse.

## Diferencia entre retencion/purga y downsampling

| Caracteristica | Retencion/Purga | Downsampling |
|---|---|---|
| Que hace | Elimina datos mas antiguos que N dias | Reemplaza N snapshots por un promedio horario |
| Los datos se pierden? | Si, completamente | Parcialmente — se pierde la resolucion, no el promedio |
| Reversible? | No | No |
| Espacio liberado | Alto | Moderado |
| Util para | Datos que ya no interesan | Datos historicos que siguen siendo relevantes |

## Cuando usar cada uno

### Usar retencion/purga cuando:

- Los datos antiguos no tienen valor historico
- Necesitas liberar la mayor cantidad de espacio posible
- Solo te interesan datos recientes (ultimos dias o semanas)

### Usar downsampling cuando:

- Quieres mantener tendencias historicas a largo plazo
- Necesitas ver como evolucionaron las metricas en meses o anos
- El espacio en disco es limitado pero no quieres perder el historial completo

### Usar ambos combinados:

- Mantener datos completos los ultimos 90 dias (retencion)
- Compactar datos mas antiguos a resolucion horaria (downsampling)

## Configuracion en ChuspaPulse.toml

    [storage]
    habilitado = true
    ruta = "/var/lib/ChuspaPulse/datos.db"
    retencion_dias = 90
    downsampling = true
    downsampling_intervalo = "1h"

Con esta configuracion:
- Los datos de los ultimos 90 dias se conservan con resolucion completa
- Los datos mas antiguos se compactan a un promedio por hora
- La tabla snapshots_horarios almacena los promedios compactados

## Consultar datos compactados

Los datos compactados se almacenan en la tabla snapshots_horarios de la base de datos SQLite. Para consultarlos:

    sqlite3 /var/lib/ChuspaPulse/datos.db

Consultar promedios horarios de CPU del ultimo mes:

    SELECT
      datetime(hora, 'unixepoch') as fecha,
      cpu_promedio,
      memory_promedio
    FROM snapshots_horarios
    WHERE hora > strftime('%s', 'now', '-30 days')
    ORDER BY hora DESC;

Consultar el maximo de CPU por dia:

    SELECT
      date(hora, 'unixepoch') as dia,
      MAX(cpu_promedio) as cpu_max
    FROM snapshots_horarios
    GROUP BY dia
    ORDER BY dia DESC;

## Verificar el estado del downsampling

Verificar el tamano de la base de datos antes y despues:

    du -sh /var/lib/ChuspaPulse/datos.db

Contar snapshots originales vs compactados:

    sqlite3 /var/lib/ChuspaPulse/datos.db "SELECT COUNT(*) FROM snapshots;"
    sqlite3 /var/lib/ChuspaPulse/datos.db "SELECT COUNT(*) FROM snapshots_horarios;"

## Advertencias

- Activar el downsampling en una base de datos existente procesara todos los datos anteriores al periodo de retencion en el proximo ciclo
- No desactivar el downsampling mientras haya datos en snapshots_horarios ya que podria generar inconsistencias
- Hacer un respaldo de la base de datos antes de activar el downsampling por primera vez:

    cp /var/lib/ChuspaPulse/datos.db /var/lib/ChuspaPulse/datos.db.bak
