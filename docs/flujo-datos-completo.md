# Flujo de datos completo del sistema ChuspaPulse

## Introducción

Este documento describe el flujo completo de datos dentro de ChuspaPulse, desde la obtención de métricas del sistema operativo hasta su exposición mediante la API HTTP para clientes externos.

El objetivo es mostrar cómo interactúan los distintos componentes del sistema y cómo viajan las métricas a través de cada etapa del pipeline.

---

## Fuentes de datos

Los collectors obtienen información directamente del sistema operativo mediante archivos virtuales del sistema Linux y llamadas a APIs del sistema.

Fuentes utilizadas:

- `/proc/stat` (CPU y procesos)
- `/proc/loadavg` (promedio de carga)
- `/proc/uptime` (tiempo de actividad)
- `/proc/meminfo` (memoria RAM)
- `/sys/class/thermal/thermal_zone0/temp` (temperatura)
- `statvfs()` (uso de disco)
- Interfaces de red del sistema

---

## Flujo completo de datos

```text
+------------------------------------------------------+
|                FUENTES DEL SISTEMA                   |
+------------------------------------------------------+
| /proc/stat                                           |
| /proc/meminfo                                        |
| /proc/loadavg                                        |
| /proc/uptime                                         |
| /sys/class/thermal/...                               |
| statvfs()                                            |
| Interfaces de red                                    |
+------------------------------------------------------+
                         |
                         v
+------------------------------------------------------+
|                     COLLECTORS                       |
+------------------------------------------------------+
| CPU Collector                                        |
| RAM Collector                                        |
| Disk Collector                                       |
| Network Collector                                    |
| Load Average Collector                               |
| Process Collector                                    |
| Temperature Collector                                |
| Uptime Collector                                     |
+------------------------------------------------------+
                         |
                         v
+------------------------------------------------------+
|             SystemMonitor::collectAll()              |
+------------------------------------------------------+
| Combina las métricas recolectadas y construye        |
| un Snapshot unificado.                               |
+------------------------------------------------------+
                         |
                         v
+------------------------------------------------------+
|                      SAMPLER                         |
+------------------------------------------------------+
| Ejecuta la recolección periódicamente según          |
| la configuración del sistema.                        |
+------------------------------------------------------+
                         |
                         v
+------------------------------------------------------+
|                  Storage::save()                     |
+------------------------------------------------------+
| Guarda los snapshots en la base de datos para        |
| consultas históricas y análisis posteriores.         |
+------------------------------------------------------+
                         |
                         v
+------------------------------------------------------+
|                    HTTP SERVER                       |
+------------------------------------------------------+
                         |
        +----------------+----------------+
        |                |                |
        v                v                v

   +-----------+   +-----------+   +-----------+
   | /metrics  |   | /history  |   | /health   |
   +-----------+   +-----------+   +-----------+

        |
        +----------------------------------+
        |                                  |
        v                                  v

   +-----------+                    +-----------+
   | /config   |                    | /alerts   |
   +-----------+                    +-----------+

        |
        v

   +----------------------+
   | /metrics/prometheus  |
   +----------------------+

                         |
                         v
+------------------------------------------------------+
|                 CLIENTES HTTP                        |
+------------------------------------------------------+
| Navegadores web                                      |
| Dashboards                                           |
| Scripts                                              |
| Herramientas de monitoreo                            |
| Sistemas externos                                    |
+------------------------------------------------------+
```

---

## Descripción de cada etapa

### 1. Fuentes de datos

Las métricas se obtienen directamente del sistema operativo Linux mediante archivos virtuales ubicados en `/proc`, información térmica del sistema y llamadas a funciones del sistema como `statvfs()`.

Estas fuentes proporcionan información actualizada sobre el estado del equipo.

---

### 2. Collectors

Cada collector es responsable de una categoría específica de métricas.

Ejemplos:

- CPU
- RAM
- Disco
- Red
- Procesos
- Temperatura
- Uptime
- Load Average

Cada collector transforma los datos del sistema en métricas utilizables por el resto de la aplicación.

---

### 3. SystemMonitor::collectAll()

El componente `SystemMonitor` coordina la recolección de métricas registradas y construye un `Snapshot` que representa el estado del sistema en un momento determinado.

---

### 4. Sampler

El `Sampler` ejecuta la recolección de métricas de forma periódica utilizando un hilo dedicado.

La frecuencia de muestreo depende de la configuración del sistema.

---

### 5. Storage

El componente `Storage` almacena los snapshots generados por el sistema.

Sus responsabilidades incluyen:

- Guardar métricas.
- Recuperar el último snapshot.
- Consultar históricos.
- Calcular promedios.
- Exportar información.

---

### 6. HTTP Server

El servidor HTTP expone las métricas y configuraciones mediante endpoints accesibles por clientes externos.

---

### 7. Handlers HTTP

#### `/metrics`

Devuelve las métricas actuales del sistema.

#### `/history`

Devuelve métricas históricas almacenadas.

#### `/health`

Permite verificar el estado general del servicio.

#### `/config`

Expone información relacionada con la configuración activa.

#### `/alerts`

Permite consultar información relacionada con alertas del sistema.

#### `/metrics/prometheus`

Expone métricas en formato compatible con Prometheus.

---

### 8. Clientes HTTP

Los datos pueden ser consumidos por:

- Navegadores web.
- Herramientas de monitoreo.
- Dashboards.
- Scripts automatizados.
- Sistemas externos.

Estos clientes realizan solicitudes HTTP y reciben la información procesada por ChuspaPulse.
