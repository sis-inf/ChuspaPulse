# Documentación de API

## Base URL

http://localhost:PUERTO

## Endpoints

```md
### GET /metrics
> **Estado:** Planeado / No disponible aún
**Descripción:**
Devuelve las métricas actuales del sistema monitoreado. (Endpoint en desarrollo)

**Parámetros opcionales:**

* `format` (string): formato de respuesta.

**Ejemplo de request:**

```bash
curl http://localhost:8080/metrics
```

**Respuesta:**

```json
{
  "cpu": 21.4,
  "memory": 58.2,
  "disk": 71.8,
  "timestamp": "2026-06-07T12:30:00Z"
}
```

**Códigos de respuesta:**

* 200 OK
* 500 Internal Server Error

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

> **Estado:** Planeado / No disponible aún

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
> **Estado:** Planeado / No disponible aún

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
Expone las métricas en formato compatible con Prometheus.