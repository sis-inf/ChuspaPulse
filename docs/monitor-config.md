# Relación entre Config y MonitorConfig

## Contexto

En el proyecto coexisten dos estructuras de configuración con responsabilidades aparentemente similares pero con propósitos y orígenes distintos: `Config` y `MonitorConfig`. Esta duplicación no está documentada, lo que genera confusión entre nuevos contribuidores. El presente documento aclara sus diferencias, solapamientos y propone una posible evolución futura.

---

## Estructura `Config`

- **Propósito**: Configuración completa del servidor, sampler y almacenamiento. Define el comportamiento general del sistema en modo normal.
- **Origen de los datos**: Archivo `pulso.toml` (u otro formato TOML) cargado al inicio.
- **Ámbito de uso**: Utilizada por el servidor principal, el sampler y los módulos de almacenamiento durante toda la ejecución.
- **Campos principales** (ejemplo, según el fragmento del archivo):
  - `intervalo_recoleccion` – intervalo entre lecturas (en segundos).
  - `nivel_registro` – nivel de log (debug, info, etc.).
  - Sección `[alertas]` con umbrales para CPU, RAM y disco, y condiciones de disparo (lecturas consecutivas, rutas, etc.).
- **Responsabilidad**: Gestionar el monitoreo continuo y las alertas del sistema en funcionamiento normal.

---

## Estructura `MonitorConfig`

- **Propósito**: Configuración específica para el modo de monitoreo puntual invocado desde la línea de comandos con el flag `--metrics`.
- **Origen de los datos**: Argumentos de la CLI (o valores por defecto), no desde archivo.
- **Ámbito de uso**: Solo durante la ejecución del comando `pulso --metrics ...`; no afecta al servidor ni al sampler.
- **Campos principales** (estructura propuesta actualmente):
  - `interval_ms` – intervalo entre lecturas en milisegundos.
  - `cpu` – booleano para habilitar monitoreo de CPU.
  - `ram` – booleano para habilitar monitoreo de memoria RAM.
  - `disk` – booleano para habilitar monitoreo de disco.
- **Responsabilidad**: Ofrecer una forma rápida y ligera de obtener métricas bajo demanda, sin necesidad de un archivo de configuración completo.

---

## Comparativa

| Aspecto               | `Config`                                    | `MonitorConfig`                            |
|-----------------------|---------------------------------------------|--------------------------------------------|
| **Origen**            | Archivo `pulso.toml`                        | Argumentos de CLI (`--metrics`)            |
| **Formato de tiempo** | `intervalo_recoleccion` en segundos         | `interval_ms` en milisegundos              |
| **Métricas**          | Definidas mediante umbrales y alertas       | Activación booleana por recurso (cpu/ram/disk) |
| **Uso**               | Servidor, sampler, storage                  | Solo comando `--metrics`                   |
| **Persistencia**      | Persistente (archivo)                       | Efímera (solo durante la ejecución del CLI) |

---

## Solapamiento identificado

Ambas estructuras controlan aspectos del monitoreo, lo que genera redundancia:

- **Intervalos**: `Config` usa `intervalo_recoleccion` (segundos), `MonitorConfig` usa `interval_ms` (milisegundos). Hay duplicación de concepto con diferente unidad.
- **Selección de métricas**: `Config` lo hace mediante alertas y umbrales (ej. `[alertas.cpu]`), mientras que `MonitorConfig` usa flags booleanos. No hay una correspondencia directa, pero ambas deciden qué se monitorea.
- **Responsabilidad**: En la práctica, ambos definen parámetros de monitoreo, pero en contextos distintos. Esto puede llevar a confusión sobre cuál usar y por qué.

---

## Propuesta de unificación futura (sin cambios de código por ahora)

Para eliminar la duplicación y hacer el sistema más intuitivo, se sugiere:

1. **Unificar en una única estructura** que pueda ser alimentada tanto desde TOML como desde CLI, usando un patrón de fábrica o builders.
2. **Estandarizar unidades** (por ejemplo, usar siempre milisegundos) y nombres de campos.
3. **Hacer que `MonitorConfig` sea un subconjunto de `Config`** o que herede de ella, reutilizando la misma definición de intervalos y habilitación de métricas.
4. **Documentar claramente** que el modo `--metrics` solo sobrescribe ciertos parámetros de `Config` para su ejecución puntual, en lugar de ser una estructura separada.

Esta evolución permitirá reducir el mantenimiento y facilitará la comprensión del código.

---

## Nota final

Este documento es puramente aclaratorio y **no implica cambios inmediatos en el código fuente**. Su objetivo es orientar a contribuidores y sentar las bases para una futura refactorización si el equipo lo decide. 