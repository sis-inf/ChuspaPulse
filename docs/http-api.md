GET /version
Método

GET

URL

/version

> **Estado:** Planeado / No disponible aún

Descripción

Devuelve la versión actual del servidor Pulso.

Parámetros

No requiere parámetros.

Ejemplo de request
GET /version HTTP/1.1
Host: localhost:8080
Ejemplo de respuesta
{
  "version": "v1.2.3",
  "commit": "abc123def",
  "buildDate": "2026-09-08T17:08:45Z"
}

GET /metrics/prometheus
Método

GET

URL

/metrics/prometheus

Descripción

Expone las métricas del sistema en el formato de texto de Prometheus, para ser recolectadas por un servidor Prometheus.

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

No requiere parámetros.

Ejemplo de request
GET /metrics/prometheus HTTP/1.1
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
