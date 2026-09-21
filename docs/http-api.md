API HTTP - Pulso

Este documento describe los endpoints HTTP disponibles en el servidor Pulso. Incluye métodos, rutas, parámetros, ejemplos de solicitud y respuesta, y códigos de estado HTTP.

GET /metrics
Método

GET

URL

/metrics

> **Estado:** Planeado / No disponible aún

Descripción

Devuelve las métricas actuales del sistema en tiempo real.

Parámetros

No requiere parámetros.

Ejemplo de request
GET /metrics HTTP/1.1
Host: localhost:8080
Ejemplo de respuesta
{
  "cpu_usage": 27.8,
  "memory_usage": 61.4,
  "disk_usage": 48.9,
  "timestamp": "2026-06-22T15:45:10Z"
}
GET /metrics/history
Método

GET

URL

/metrics/history

Descripción

Devuelve el historial de métricas en un rango de tiempo.

Parámetros de query
Parámetro	Tipo	Requerido	Descripción
from	string (ISO 8601)	sí	Inicio del rango
to	string (ISO 8601)	sí	Fin del rango
Ejemplo de request
GET /metrics/history?from=2026-06-22T10:00:00Z&to=2026-06-22T11:00:00Z HTTP/1.1
Host: localhost:8080
Ejemplo de respuesta
{
  "data": [
    {
      "timestamp": "2026-06-22T10:15:00Z",
      "cpu_usage": 35.2,
      "memory_usage": 70.0
    },
    {
      "timestamp": "2026-06-22T10:30:00Z",
      "cpu_usage": 33.9,
      "memory_usage": 68.7
    }
  ]
}
GET /health
Método

GET

URL

/health

Descripción

Verifica el estado del servidor.

Ejemplo de request
GET /health HTTP/1.1
Host: localhost:8080
Ejemplo de respuesta
{
  "status": "ok",
  "uptime_seconds": 18452
}
GET /config
Método

GET

URL

/config

> **Estado:** Planeado / No disponible aún
Descripción

Devuelve la configuración activa del sistema (sin datos sensibles).

Ejemplo de request
GET /config HTTP/1.1
Host: localhost:8080
Ejemplo de respuesta
{
  "refresh_interval": 5,
  "max_history_points": 500,
  "alerts_enabled": true
}
GET /alerts
Método

GET

URL

/alerts
> **Estado:** Planeado / No disponible aún
Descripción

Devuelve las alertas activas del sistema.

Ejemplo de request
GET /alerts HTTP/1.1
Host: localhost:8080
Ejemplo de respuesta
{
  "alerts": [
    {
      "id": 101,
      "type": "CPU_HIGH",
      "message": "CPU por encima del 90%",
      "timestamp": "2026-06-22T15:40:00Z"
    }
  ]
}
Códigos de respuesta HTTP (global)
Código	Significado
200	OK
400	Solicitud inválida
403	No autorizado
404	Recurso no encontrado
500	Error interno del servidor
503	Servicio no disponible

GET /version
Método
GET
URL
/version
> **Estado:** Disponible / Funcional

Descripción
Devuelve la versión actual de la aplicación.

GET /metrics/prometheus
Método
GET
URL
/metrics/prometheus
> **Estado:** Disponible / Funcional

Descripción
Expone las métricas en formato compatible con Prometheus.