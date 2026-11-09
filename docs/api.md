# Documentación de API

## Base URL

http://localhost:PUERTO

## Endpoints

```md
### GET /metrics
> **Estado:** Disponible / Funcional

**Descripción:**
Expone las métricas actuales del servicio: el snapshot más reciente que el
sampler guardó en el almacenamiento, en el formato de exposición de texto de
Prometheus. Es la ruta que debe configurarse como objetivo de recolección
(`scrape`) en Prometheus. Devuelve el mismo contenido que
[`GET /metrics/prometheus`](#get-metricsprometheus).

**Ejemplo de request:**

```bash
curl -i http://localhost:8080/metrics
```

**Respuesta (200 OK, `Content-Type: text/plain; version=0.0.4`):**

```text
# HELP chuspapulse_cpu_usage Metrica de chuspapulse
# TYPE chuspapulse_cpu_usage gauge
chuspapulse_cpu_usage 21.5

# HELP chuspapulse_memory_used Metrica de chuspapulse
# TYPE chuspapulse_memory_used gauge
chuspapulse_memory_used 8589934592
```

**Métricas expuestas:**

Cada nombre se forma con el prefijo `chuspapulse_` y el nombre interno de la
métrica, reemplazando los puntos por guiones bajos. Todas son de tipo `gauge`.

| Métrica Prometheus | Nombre interno | Unidad |
|---|---|---|
| `chuspapulse_cpu_usage` | `cpu.usage` | Porcentaje |
| `chuspapulse_cpu_cores` | `cpu.cores` | Núcleos |
| `chuspapulse_memory_total` | `memory.total` | Bytes |
| `chuspapulse_memory_used` | `memory.used` | Bytes |
| `chuspapulse_memory_available` | `memory.available` | Bytes |
| `chuspapulse_disk_total` | `disk.total` | Bytes |
| `chuspapulse_disk_used` | `disk.used` | Bytes |
| `chuspapulse_disk_free` | `disk.free` | Bytes |
| `chuspapulse_network_rx_bytes` | `network.rx_bytes` | Bytes |
| `chuspapulse_network_tx_bytes` | `network.tx_bytes` | Bytes |

**Ejemplo de configuración de Prometheus:**

```yaml
scrape_configs:
  - job_name: chuspapulse
    metrics_path: /metrics
    static_configs:
      - targets: ["localhost:8080"]
```

**Códigos de respuesta:**

* 200 OK: métricas del último snapshot.
* 503 Service Unavailable: todavía no hay ningún snapshot guardado. Cuerpo: `{"error":"no data available"}`.
* 500 Internal Server Error: falló la lectura del almacenamiento. Cuerpo: `{"error":"error interno"}`.

### GET /metrics/history

**Descripción:**
Obtiene el historial de métricas registradas.

**Parámetros opcionales:**

* `limit` (integer): cantidad máxima de registros.

**Ejemplo de request:**

```bash
curl "http://localhost:8080/metrics/history?limit=5"
```

**Respuesta:**

```json
{
  "history": [
    {
      "cpu": 18.5,
      "memory": 52.0,
      "timestamp": "2026-06-07T12:00:00Z"
    },
    {
      "cpu": 21.4,
      "memory": 58.2,
      "timestamp": "2026-06-07T12:30:00Z"
    }
  ]
}
```

**Códigos de respuesta:**

* 200 OK
* 400 Bad Request
* 500 Internal Server Error

### GET /health

**Descripción:**
Verifica el estado general del servicio.

**Ejemplo de request:**

```bash
curl http://localhost:8080/health
```

**Respuesta:**

```json
{
  "status": "UP",
  "uptime": 86400
}
```

**Códigos de respuesta:**

* 200 OK
* 500 Internal Server Error

### GET /config

> **Estado:** Disponible / Funcional

**Descripción:**
Devuelve la configuración activa del servidor.

**Ejemplo de request:**

```bash
curl http://localhost:8080/config
```

**Respuesta:**

```json
{
  "port": 8080,
  "refreshInterval": 5,
  "alertsEnabled": true
}
```

**Códigos de respuesta:**

* 200 OK
* 500 Internal Server Error

### GET /alerts
> **Estado:** Disponible / Funcional

**Descripción:**
Lista las alertas activas generadas por el sistema.

**Ejemplo de request:**

```bash
curl http://localhost:8080/alerts
```

**Respuesta:**

```json
{
  "alerts": [
    {
      "id": 1,
      "severity": "HIGH",
      "message": "Uso de CPU superior al 90%"
    }
  ]
}
```

**Códigos de respuesta:**

* 200 OK
* 500 Internal Server Error


## Códigos de error

| Código | Descripción |
|---|---|
| 200 | OK |
| 400 | Bad Request |
| 500 | Internal Server Error |

### GET /version

> **Estado:** Disponible / Funcional
**Descripción:**
Devuelve la versión actual de la aplicación.

---

### GET /metrics/prometheus
> **Estado:** Disponible / Funcional

**Descripción:**
Expone las métricas en formato compatible con Prometheus. Devuelve el mismo
contenido que [`GET /metrics`](#get-metrics); si todavía no hay datos responde
`200` con el cuerpo vacío.