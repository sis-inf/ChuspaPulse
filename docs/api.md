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

| Parámetro | Tipo | Rango válido | Por defecto |
|---|---|---|---|
| `desde` | integer (timestamp Unix) | Mayor o igual a 0 | Hace 1 hora |
| `hasta` | integer (timestamp Unix) | 0 (sin límite) o mayor o igual a `desde` | 0 |
| `limite` | integer | 1 a 10000 | 1000 |

**Ejemplo de request:**

```bash
curl "http://localhost:8080/metrics/history?limite=5"
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
* 400 Bad Request: algún parámetro no es un entero o está fuera de rango.
* 500 Internal Server Error: falló la consulta al almacenamiento o la serialización.

**Ejemplos de error:**

```text
GET /metrics/history?limite=abc
400 {"error":"parametro invalido: limite debe ser un entero entre 1 y 10000"}

GET /metrics/history?desde=-1
400 {"error":"parametro invalido: desde debe ser un entero mayor o igual a 0"}

GET /metrics/history?desde=200&hasta=100
400 {"error":"parametro invalido: hasta debe ser 0 o un entero mayor o igual a desde"}

GET /metrics/history (con la base de datos no disponible)
500 {"error":"error interno"}
```

### GET /health

**Descripción:**
Chequeo básico de salud (liveness): indica que el proceso del agente está vivo
y respondiendo. No verifica los componentes internos; para eso usar
[`GET /ready`](#get-ready).

**Ejemplo de request:**

```bash
curl http://localhost:8080/health
```

**Respuesta:**

```json
{
  "status": "ok",
  "uptime_seconds": 86400,
  "version": "0.1.0",
  "hostname": "servidor-01"
}
```

**Códigos de respuesta:**

* 200 OK
* 500 Internal Server Error

### GET /ready

**Descripción:**
Chequeo de readiness: indica si el servicio está preparado para recibir
solicitudes. Evalúa los componentes requeridos y responde 503 si alguno no está
disponible.

| Componente | Disponible cuando |
|---|---|
| `storage` | La base de datos SQLite responde a una consulta. |
| `sampler` | El bucle de muestreo de métricas está en ejecución. |

**Ejemplo de request:**

```bash
curl -i http://localhost:8080/ready
```

**Respuesta (200 OK):**

```json
{
  "status": "ready",
  "componentes": {
    "storage": true,
    "sampler": true
  }
}
```

**Respuesta (503 Service Unavailable):**

```json
{
  "status": "not_ready",
  "componentes": {
    "storage": false,
    "sampler": true
  }
}
```

**Códigos de respuesta:**

* 200 OK: todos los componentes requeridos están disponibles.
* 503 Service Unavailable: al menos un componente no está disponible.

**Diferencia con `/health`:**

| Endpoint | Pregunta que responde | Si falla |
|---|---|---|
| `/health` | ¿El proceso está vivo? | El supervisor debería reiniciar el agente. |
| `/ready` | ¿Puede atender solicitudes ahora? | El supervisor debería dejar de enviarle tráfico hasta que vuelva a 200. |

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


### GET /alerts/history

**Descripción:**
Obtiene el historial de alertas persistido, ordenado por timestamp descendente.

**Parámetros opcionales:**

| Parámetro | Tipo | Rango válido | Por defecto |
|---|---|---|---|
| `limite` | integer | 1 a 1000 | 100 |
| `offset` | integer | Mayor o igual a 0 | 0 |

**Ejemplo de request:**

```bash
curl "http://localhost:8080/alerts/history?limite=10&offset=0"
```

**Respuesta:**

```json
[
  {
    "id": 1,
    "timestamp": 1780833600,
    "metrica": "cpu",
    "umbral": 80.0,
    "valor": 91.5,
    "mensaje": "CPU por encima del umbral"
  }
]
```

**Códigos de respuesta:**

* 200 OK
* 400 Bad Request: algún parámetro no es un entero o está fuera de rango.
* 500 Internal Server Error: falló la obtención del historial o la serialización.

**Ejemplos de error:**

```text
GET /alerts/history?limite=0
400 {"error":"parametro invalido: limite debe ser un entero entre 1 y 1000"}

GET /alerts/history?offset=-1
400 {"error":"parametro invalido: offset debe ser un entero mayor o igual a 0"}
```

## Códigos de error

| Código | Descripción |
|---|---|
| 200 | OK |
| 400 | Bad Request: la solicitud tiene parámetros inválidos. El cliente debe corregirla antes de reintentar. |
| 500 | Internal Server Error: la solicitud era válida pero el servidor falló al procesarla. |
| 503 | Service Unavailable: `/ready` con algún componente requerido no disponible. |

En los endpoints de historial, las respuestas de error usan siempre el formato
`{"error":"<mensaje>"}` con `Content-Type: application/json`. Los errores de
entrada indican qué parámetro es inválido y su rango; los errores internos no
exponen detalles de la causa.

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