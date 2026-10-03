# SLA y Precisión de las Métricas

Este documento describe la precisión esperada de las métricas recolectadas por el sistema Pulso, los límites conocidos y cómo interpretar los valores expuestos.

## Precisión por tipo de métrica

### CPU

- **Fuente de datos**: `/proc/stat` (Linux).
- **Resolución**: la métrica se basa en el tiempo de CPU consumido por cada núcleo, expresado en *jiffies* (unidad de tiempo del kernel). La duración de un *jiffie* depende de la configuración del kernel (`CONFIG_HZ`), normalmente 100 Hz (10 ms) o 250 Hz (4 ms).
- **Precisión**: la diferencia entre dos lecturas consecutivas tiene un error de ±1 *jiffie* por núcleo. En sistemas con carga alta, el intervalo entre lecturas puede verse afectado por la programación de procesos, lo que introduce un **jitter** de hasta varios milisegundos.
- **Cálculo de porcentaje**: el uso de CPU se calcula como la fracción de tiempo activo respecto al intervalo total. Dado que el intervalo se mide en tiempo real (reloj de pared), el error relativo puede ser significativo para intervalos muy cortos (ej. < 1 s). Para intervalos ≥ 5 s, el error suele ser < 0.5%.

### Memoria RAM

- **Fuente de datos**: `/proc/meminfo`.
- **Resolución**: los valores se reportan en kilobytes (KB). La granularidad depende de la página de memoria (normalmente 4 KB).
- **Precisión**: las métricas como `MemAvailable`, `MemFree` y `Buffers` son instantáneas en el momento de la lectura. No hay acumulación de errores, pero el valor puede fluctuar rápidamente debido a la caché de páginas y la actividad de swap. En sistemas con alta E/S, la precisión de `MemAvailable` puede verse afectada por la estimación del kernel, con un margen de ±5% en condiciones extremas.

### Disco (I/O)

- **Fuente de datos**: `/proc/diskstats`.
- **Resolución**: los contadores de lectura/escritura y sectores transferidos son acumulativos. Cada operación de I/O incrementa los contadores atómicamente, pero la lectura de los contadores no es instantánea; puede haber una pequeña demora entre la lectura de cada campo, lo que puede causar inconsistencias si el kernel actualiza los valores durante la lectura.
- **Precisión**: el ancho de banda (bytes/s) se calcula a partir de la diferencia de contadores entre dos muestras. El error depende de la frecuencia de muestreo y de la variabilidad de la carga. Con un intervalo de 15 s, el error típico es inferior al 1% para cargas estables, pero puede superar el 10% si hay picos de operaciones muy cortos (ej. < 1 s) que no se capturan completamente.

## Sampler y frecuencia de muestreo

El sistema recolecta métricas en intervalos regulares definidos por la variable de configuración `collector.interval` (por defecto, 15 segundos). Este intervalo es configurable en el archivo de configuración o mediante variables de entorno.

### Jitter del sampler

El sampler se ejecuta en un goroutine que duerme entre recolecciones. Debido a la planificación del sistema operativo y a la carga de la CPU, el tiempo real entre recolecciones puede variar ligeramente (jitter). En condiciones normales, el jitter es inferior a 50 ms, pero bajo alta carga puede superar los 200 ms.

Este jitter afecta directamente al cálculo de tasas (como CPU o I/O), ya que el intervalo real no es exactamente el configurado. Para mitigarlo, el sistema registra la marca de tiempo real de cada muestra y utiliza esa diferencia para los cálculos, en lugar del intervalo nominal.

### Redondeo y formato

Los valores numéricos se redondean a un decimal (ej. 27.8%) para reducir el ruido visual. El redondeo introduce un error de ±0.05% en el valor mostrado, que es despreciable frente a otras fuentes de error.

## Interpretación de los límites

Al leer las métricas expuestas por los endpoints (`/metrics`, `/metrics/history`, `/metrics/prometheus`), tenga en cuenta:

- Las métricas son **instantáneas** o **promedios** en el intervalo de muestreo. No representan valores pico.
- Para CPU e I/O, los valores pueden mostrar pequeñas variaciones entre muestras consecutivas debido al jitter y al redondeo. Si observa diferencias de ±1-2% en un sistema estable, es normal.
- Para RAM, los valores de `MemAvailable` pueden diferir de la memoria libre real debido a la caché. Consulte la documentación del kernel para entender mejor esta métrica.
- Si necesita mayor precisión (por ejemplo, para facturación o SLA estricto), reduzca el intervalo de muestreo (por ejemplo, a 5 s) y promedie varias muestras para suavizar el jitter. Sin embargo, tenga en cuenta que intervalos muy cortos (< 1 s) pueden aumentar el ruido debido a la resolución de los contadores.

## Consideraciones adicionales

- El sistema está diseñado para entornos con carga moderada (hasta 1000 muestras por minuto). En cargas extremas, el sampler puede retrasarse, lo que aumenta el jitter y reduce la precisión.
- Para entornos críticos, se recomienda monitorear también el tiempo de ejecución del sampler (expuesto como métrica interna) para detectar posibles degradaciones.

---

