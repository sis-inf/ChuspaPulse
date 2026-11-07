# Integración de ChuspaPulse con Grafana

## Introducción

ChuspaPulse permite exponer métricas en un formato compatible con Prometheus, lo que permite utilizar herramientas de visualización como Grafana para crear paneles de monitoreo.

La integración utiliza el siguiente flujo:

```text
ChuspaPulse → Prometheus → Grafana
```

ChuspaPulse expone sus métricas mediante un endpoint HTTP compatible con Prometheus. Prometheus recopila estas métricas y Grafana las utiliza como fuente de datos para generar visualizaciones.

## Requisitos

Antes de configurar Grafana se necesita:

- Una instancia de ChuspaPulse ejecutándose.
- Prometheus instalado y configurado.
- Una instancia de Grafana instalada.

## Configuración de Prometheus

ChuspaPulse expone sus métricas mediante el endpoint:

```text
/metrics/prometheus
```

Por ejemplo:

```text
http://localhost:8080/metrics/prometheus
```

El proyecto incluye un ejemplo de configuración para Prometheus ubicado en:

```text
examples/prometheus.yml
```

Este archivo contiene la configuración necesaria para recopilar las métricas de ChuspaPulse:

```yaml
scrape_configs:
  - job_name: ChuspaPulse
    metrics_path: /metrics/prometheus
    static_configs:
      - targets:
          - localhost:8080
```

Para iniciar Prometheus utilizando esta configuración se puede ejecutar:

```bash
docker run -p 9090:9090 \
  -v $(pwd)/examples/prometheus.yml:/etc/prometheus/prometheus.yml \
  prom/prometheus
```

## Configurar Prometheus como fuente de datos en Grafana

Para conectar Grafana con las métricas generadas por ChuspaPulse:

1. Ingresar a la interfaz web de Grafana.
2. Acceder a la sección **Data Sources**.
3. Seleccionar **Prometheus** como fuente de datos.
4. Configurar la URL del servidor Prometheus:

```text
http://localhost:9090
```

5. Guardar la configuración y verificar la conexión.

Después de completar estos pasos, Grafana consulta las métricas almacenadas por Prometheus, que previamente fueron recopiladas desde ChuspaPulse.

## Consultas PromQL de ejemplo

### Uso de CPU

Muestra el valor actual del uso de CPU reportado por ChuspaPulse:

```promql
ChuspaPulse_cpu_usage
```

### Uso de memoria RAM

Muestra el uso actual de memoria RAM:

```promql
ChuspaPulse_memory_usage
```

### Tráfico recibido

Muestra el tráfico recibido medido por ChuspaPulse:

```promql
ChuspaPulse_rx_bytes
```

## Métricas disponibles

ChuspaPulse exporta las siguientes métricas compatibles con Prometheus:

| Métrica | Descripción |
|---------|-------------|
| `ChuspaPulse_cpu_usage` | Uso de CPU |
| `ChuspaPulse_memory_usage` | Uso de memoria RAM |
| `ChuspaPulse_disk_usage` | Uso de disco |
| `ChuspaPulse_rx_bytes` | Tráfico recibido |
| `ChuspaPulse_tx_bytes` | Tráfico enviado |

