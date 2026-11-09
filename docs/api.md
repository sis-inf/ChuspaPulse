# Documentación de API

## Base URL

http://localhost:PUERTO

## Endpoints

```md
### GET /metrics
> **Estado:** Disponible / Funcional
**Descripción:**
Devuelve el último snapshot. 

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
Expone las métricas en formato compatible con Prometheus.