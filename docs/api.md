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


## Códigos de error

| Código | Descripción |
|---|---|
| 200 | OK |
| 400 | Bad Request |
| 500 | Internal Server Error |
| 503 | Service Unavailable (`/ready` con algún componente no disponible) |

### GET /version

> **Estado:** Disponible / Funcional
**Descripción:**
Devuelve la versión actual de la aplicación.

---

### GET /metrics/prometheus
> **Estado:** Disponible / Funcional

**Descripción:**
Expone las métricas en formato compatible con Prometheus.