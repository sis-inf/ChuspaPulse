# Relación entre Config y MonitorConfig

## Contexto

En el proyecto coexisten dos estructuras de configuración con responsabilidades aparentemente similares pero con propósitos y orígenes distintos: `Config` y `MonitorConfig`. Esta duplicación no está documentada, lo que genera confusión entre nuevos contribuidores. El presente documento aclara sus diferencias, solapamientos y propone una posible evolución futura.

---

## Estructura `Config`

- **Propósito**: Configuración completa del servidor, sampler y almacenamiento. Define el comportamiento general del sistema en modo normal.
- **Origen de los datos**: Archivo `pulso.toml` (ruta por defecto; se puede cambiar con `--config <ruta>`) y variables de entorno `PULSO_*`, que tienen prioridad sobre el archivo. Si el archivo no existe, se usan los valores por defecto.
- **Ámbito de uso**: Utilizada por el servidor principal, el sampler y los módulos de almacenamiento durante toda la ejecución.
- **Campos principales**:
  - `servidor.host` y `servidor.puerto` – dirección y puerto del servidor HTTP.
  - `sampler.intervalo_segundos` – intervalo entre lecturas (en segundos).
  - `storage.ruta_db` – ruta de la base de datos SQLite.
  - `umbrales.cpu_max`, `umbrales.ram_max`, `umbrales.disco_max` – límites de alerta.
  - `nivel_log` – nivel de log (`debug`, `info`, `warn`, `error`).
  - `output_format` – formato de salida (`json`, `csv`, `prometheus`).
- **Responsabilidad**: Gestionar el monitoreo continuo y las alertas del sistema en funcionamiento normal.

---

## Estructura `MonitorConfig`

- **Propósito**: Definir desde la línea de comandos qué collectors de CPU, RAM y disco se activan (`--metrics`) y un intervalo en milisegundos (`--interval`).
- **Origen de los datos**: Argumentos de la CLI (o valores por defecto), no desde archivo.
- **Ámbito de uso**: Solo se llena desde la CLI, pero la lista de collectors que decide se usa tanto en `--once` como en el modo servidor.
- **Campos principales** (valores por defecto entre paréntesis):
  - `interval_ms` (`1000`) – intervalo en milisegundos, se pasa con `--interval <ms>` y debe ser mayor a 100.
  - `cpu` (`true`) – habilita el collector de CPU.
  - `ram` (`true`) – habilita el collector de RAM.
  - `disk` (`true`) – habilita el collector de disco.
- **Responsabilidad**: Permitir elegir las métricas principales de una ejecución sin editar `pulso.toml`. Con `--metrics cpu,ram` se activan solo CPU y RAM y se desactiva disco. Sin `--metrics`, los tres quedan activos. El resto de collectors (batería, red, procesos, loadavg, uptime, temperatura) se registran siempre.

---

## Comparativa


| Aspecto               | `Config`                                    | `MonitorConfig`                               |
|-----------------------|---------------------------------------------|-----------------------------------------------|
| **Origen**            | Archivo `pulso.toml` y variables `PULSO_*`  | Argumentos de CLI (`--metrics`, `--interval`) |
| **Formato de tiempo** | `sampler.intervalo_segundos` en segundos    | `interval_ms` en milisegundos                 |
| **Métricas**          | Límites de alerta en `[umbrales]`           | Activación booleana por recurso (cpu/ram/disk) |
| **Uso**               | Servidor, sampler, storage, alertas         | Selección de collectors (`--once` y servidor) |
| **Persistencia**      | Persistente (archivo)                       | Efímera (solo durante la ejecución del proceso) |

---

## ¿Cuál usar?

- **Puerto, host, base de datos, intervalo de muestreo, umbrales de alerta o nivel de log** → `Config` (`pulso.toml` o variables `PULSO_*`).
- **Activar solo algunas métricas (`cpu`, `ram`, `disk`) en una ejecución concreta** → `MonitorConfig` (`--metrics`).
- **Un ajuste permanente nuevo** → agrégalo a `Config` y a su sección en `pulso.toml`.
- **Un flag de CLI para una ejecución puntual** → agrégalo a `CliOptions`.

---

## Solapamiento identificado

Ambas estructuras controlan aspectos del monitoreo, lo que genera redundancia:

- **Intervalos**: `Config` usa `sampler.intervalo_segundos` (segundos) y `MonitorConfig` usa `interval_ms` (milisegundos). Hoy `--interval` se valida y se guarda, pero `main.cpp` no lee `interval_ms`: el sampler usa siempre `sampler.intervalo_segundos`.
- **Selección de métricas**: `MonitorConfig` decide qué collectors corren; `Config` define los límites de alerta en `[umbrales]`. Las tres alertas (cpu, ram, disk) se registran siempre, sin consultar `MonitorConfig`.
- **Responsabilidad**: En la práctica, ambos definen parámetros de monitoreo, pero en contextos distintos. Esto puede llevar a confusión sobre cuál usar y por qué.
- **Formato de salida**: `Config.output_format` (`json`, `csv`, `prometheus`) viene de `pulso.toml`; `--format` (`json`, `csv`, `prometheus`, `text`) solo se usa con `--once`. No están conectados.

---

## Propuesta de unificación futura (sin cambios de código por ahora)

Para eliminar la duplicación y hacer el sistema más intuitivo, se sugiere:

1. **Unificar en una única estructura** que pueda ser alimentada tanto desde TOML como desde CLI, usando un patrón de fábrica o builders.
2. **Estandarizar unidades** (por ejemplo, usar siempre milisegundos) y nombres de campos.
3. **Hacer que `MonitorConfig` sea un subconjunto de `Config`** o que herede de ella, reutilizando la misma definición de intervalos y habilitación de métricas.
4. **Documentar claramente** que los flags `--metrics` e `--interval` solo sobrescriben ciertos parámetros de `Config` para una ejecución puntual, en lugar de ser una estructura separada.

Esta evolución permitirá reducir el mantenimiento y facilitará la comprensión del código.

---

## Nota final

Este documento es puramente aclaratorio y **no implica cambios inmediatos en el código fuente**. Su objetivo es orientar a contribuidores y sentar las bases para una futura refactorización si el equipo lo decide.
