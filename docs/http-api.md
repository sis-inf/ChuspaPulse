GET /version
Método

GET

URL

/version

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

Parámetros

No requiere parámetros.

Ejemplo de request
GET /metrics/prometheus HTTP/1.1
Host: localhost:8080
Ejemplo de respuesta (fragmento)

# HELP go_gc_duration_seconds A summary of the GC invocation durations.
# TYPE go_gc_duration_seconds summary
go_gc_duration_seconds{quantile="0"} 1.2e-05
go_gc_duration_seconds{quantile="0.5"} 2.3e-05
go_gc_duration_seconds{quantile="0.9"} 4.5e-05
# HELP promhttp_metric_handler_requests_total Total number of scrapes by HTTP status code.
# TYPE promhttp_metric_handler_requests_total counter
promhttp_metric_handler_requests_total{code="200"} 42