# Catálogo de métricas del agente ChuspaPulse

Este documento es la referencia de las métricas que recolecta el agente ChuspaPulse, sus unidades y los nombres con los que se exportan hacia Prometheus. Refleja el comportamiento del código de la rama `dev`.

## Convención de nombres

Cada métrica tiene un identificador interno con el formato `categoria.campo`, por ejemplo `cpu.usage`. Al exportarla en formato Prometheus, el agente aplica dos reglas:

1. Agrega el prefijo `chuspapulse_`.
2. Reemplaza cada punto (`.`) por un guion bajo (`_`).

Por ejemplo, `cpu.usage` se exporta como `chuspapulse_cpu_usage`.

Además:

- Todas las métricas se exportan con tipo `gauge`.
- La unidad no se exporta; solo se usa internamente y en las salidas JSON y de texto.
- El texto de `# HELP` es genérico: `Metrica de chuspapulse`.

## Endpoint de Prometheus

| Campo | Valor |
|-------|-------|
| Ruta | `GET /metrics/prometheus` |
| Content-Type | `text/plain; version=0.0.4` |
| Host por defecto | `0.0.0.0` |
| Puerto por defecto | `8080` |
| Intervalo de muestreo por defecto | 10 segundos |

El host y el puerto se pueden cambiar con las variables de entorno `CHUSPAPULSE_SERVIDOR_HOST` y `CHUSPAPULSE_SERVIDOR_PUERTO`, o con la sección del servidor del archivo de configuración. El intervalo de muestreo se cambia con `CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS`.

## Métricas expuestas en `/metrics/prometheus`

El endpoint entrega los datos guardados en el almacenamiento (`Storage`). Hoy el almacenamiento persiste las siguientes diez métricas, de CPU, memoria, disco y red.

| Categoría | Identificador interno | Nombre exportado | Tipo | Unidad | Descripción |
|-----------|-----------------------|------------------|------|--------|-------------|
| CPU | `cpu.usage` | `chuspapulse_cpu_usage` | gauge | Porcentaje (%) | Uso del procesador. |
| CPU | `cpu.cores` | `chuspapulse_cpu_cores` | gauge | Cantidad de núcleos | Núcleos del procesador. |
| Memoria | `memory.total` | `chuspapulse_memory_total` | gauge | Bytes | Memoria RAM física total. |
| Memoria | `memory.used` | `chuspapulse_memory_used` | gauge | Bytes | Memoria RAM en uso efectivo. |
| Memoria | `memory.available` | `chuspapulse_memory_available` | gauge | Bytes | Memoria RAM disponible para nuevas asignaciones. |
| Disco | `disk.total` | `chuspapulse_disk_total` | gauge | Bytes | Capacidad total del disco. |
| Disco | `disk.used` | `chuspapulse_disk_used` | gauge | Bytes | Espacio usado. |
| Disco | `disk.free` | `chuspapulse_disk_free` | gauge | Bytes | Espacio libre disponible para usuarios no privilegiados. |
| Red | `network.rx_bytes` | `chuspapulse_network_rx_bytes` | gauge | Bytes | Total de bytes recibidos, sumando todas las interfaces. |
| Red | `network.tx_bytes` | `chuspapulse_network_tx_bytes` | gauge | Bytes | Total de bytes enviados, sumando todas las interfaces. |

## Otras métricas que recolecta el agente

El agente recolecta además las siguientes métricas. Con el esquema actual del almacenamiento no se persisten, por lo que no aparecen en `/metrics/prometheus`. Si el formato `prometheus` se aplica directamente sobre lo recolectado, como en el modo de ejecución única, se exportan con las mismas reglas de nombres.

| Categoría | Identificador interno | Nombre exportado | Unidad | Descripción |
|-----------|-----------------------|------------------|--------|-------------|
| Carga | `loadavg.1` | `chuspapulse_loadavg_1` | Carga | Promedio de carga del sistema en 1 minuto. |
| Carga | `loadavg.5` | `chuspapulse_loadavg_5` | Carga | Promedio de carga del sistema en 5 minutos. |
| Carga | `loadavg.15` | `chuspapulse_loadavg_15` | Carga | Promedio de carga del sistema en 15 minutos. |
| Temperatura | `temperatura.cpu` | `chuspapulse_temperatura_cpu` | Grados Celsius | Temperatura de la CPU. Solo se genera si el sistema tiene un sensor disponible. |
| Uptime | `uptime.seconds` | `chuspapulse_uptime_seconds` | Segundos | Tiempo que lleva encendido el sistema. |
| Procesos | `procesos.running` | `chuspapulse_procesos_running` | Cantidad | Procesos en ejecución. |
| Procesos | `procesos.blocked` | `chuspapulse_procesos_blocked` | Cantidad | Procesos bloqueados. |
| Procesos | `procesos.total` | `chuspapulse_procesos_total` | Cantidad | Total de procesos. |
| Batería | `bateria.porcentaje` | `chuspapulse_bateria_porcentaje` | Porcentaje (%) | Carga de la batería. Solo se genera si hay batería. |
| Batería | `bateria.cargando` | `chuspapulse_bateria_cargando` | 1 o 0 | `1` si la batería se está cargando, `0` si no. |

### Activación de los colectores

Con la opción `--metrics` se eligen los colectores principales. Por ejemplo, `--metrics cpu,ram` activa solo CPU y RAM y desactiva disco. Sin `--metrics`, CPU, memoria y disco quedan activos. El resto de colectores (batería, red, procesos, carga, uptime y temperatura) se registran siempre.

## Ejemplo de salida

Respuesta de `GET /metrics/prometheus` con valores ilustrativos:

```text
# HELP chuspapulse_cpu_usage Metrica de chuspapulse
# TYPE chuspapulse_cpu_usage gauge
chuspapulse_cpu_usage 21.4

# HELP chuspapulse_memory_used Metrica de chuspapulse
# TYPE chuspapulse_memory_used gauge
chuspapulse_memory_used 4294967296

# HELP chuspapulse_network_rx_bytes Metrica de chuspapulse
# TYPE chuspapulse_network_rx_bytes gauge
chuspapulse_network_rx_bytes 1073741824
```

Para comprobarlo desde la línea de comandos:

```bash
curl http://localhost:8080/metrics/prometheus
```

## Ejemplos de raspado por Prometheus

Configuración mínima en `prometheus.yml` para raspar un agente local:

```yaml
scrape_configs:
  - job_name: "chuspapulse"
    metrics_path: "/metrics/prometheus"
    scrape_interval: 15s
    static_configs:
      - targets: ["localhost:8080"]
```

Para varios agentes, se agregan los destinos con una etiqueta que los identifique:

```yaml
scrape_configs:
  - job_name: "chuspapulse"
    metrics_path: "/metrics/prometheus"
    scrape_interval: 15s
    static_configs:
      - targets: ["servidor-a:8080"]
        labels:
          equipo: "servidor-a"
      - targets: ["servidor-b:8080"]
        labels:
          equipo: "servidor-b"
```

El intervalo de raspado conviene que no sea menor al intervalo de muestreo del agente (10 segundos por defecto), porque el endpoint entrega datos ya muestreados.

### Consultas de ejemplo (PromQL)

| Objetivo | Consulta |
|----------|----------|
| Uso de CPU actual | `chuspapulse_cpu_usage` |
| Porcentaje de memoria usada | `chuspapulse_memory_used / chuspapulse_memory_total * 100` |
| Porcentaje de disco usado | `chuspapulse_disk_used / chuspapulse_disk_total * 100` |
| Bytes recibidos por segundo | `rate(chuspapulse_network_rx_bytes[5m])` |

## Notas y limitaciones conocidas

- **Unidades:** Prometheus no recibe la unidad. Esta tabla es la única referencia de las unidades de cada métrica.
- **Tipo `gauge`:** los totales de red son acumulados que solo crecen, pero se exportan como `gauge`. Para obtener un caudal en bytes por segundo se calcula la tasa con `rate()`, como en el ejemplo anterior.
- **Nombres alternativos en el código:** algunos colectores usan nombres distintos para la misma magnitud. Los colectores de `platform/linux` usan, por ejemplo, `memory.total_bytes` y `disk.free_bytes`, y los colectores heredados usan `net.rx` y `net.tx`. El almacenamiento unifica los datos en los diez nombres de la tabla principal, que son los que se exportan por el endpoint.
- **Nombres en otros documentos:** `docs/metricas.md` usa nombres como `ram.used` y `network.rx`, que no coinciden con los que se exportan. Para Prometheus, la referencia es este documento.
