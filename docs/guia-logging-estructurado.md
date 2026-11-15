# Guía de Logging Estructurado en JSON

## Descripción

ChuspaPulse soporta un modo de logging estructurado en JSON como alternativa al texto plano. Este modo facilita la integración con agregadores de logs externos como **Loki** o **Filebeat**, ya que cada línea de log es un objeto JSON parseable.

---

## Configuración en ChuspaPulse.toml

Para activar el logging estructurado, agrega la opción `log_formato` en el archivo `ChuspaPulse.toml`:

```toml
[storage]
ruta_db = "ChuspaPulse.db"
nivel_log = "info"
log_formato = "json"
```

Los valores posibles para `log_formato` son:

| Valor | Descripción |
|-------|-------------|
| `"text"` | Texto plano legible (valor por defecto) |
| `"json"` | JSON estructurado, una línea por evento |

---

## Esquema del objeto JSON emitido

Cada línea de log en modo JSON tiene el siguiente esquema:

```json
{
  "timestamp": "2026-03-07T23:45:00Z",
  "nivel": "info",
  "mensaje": "Muestreo completado",
  "contexto": {
    "cpu": 21.4,
    "memoria": 58.2,
    "disco": 71.8
  }
}
```

### Campos

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `timestamp` | string (ISO 8601) | Fecha y hora del evento en UTC |
| `nivel` | string | Nivel de log: `debug`, `info`, `warn`, `error` |
| `mensaje` | string | Descripción del evento |
| `contexto` | object | Datos adicionales del evento (opcional) |

---

## Ejemplos de salida

### Nivel info — muestreo completado

```json
{"timestamp":"2026-03-07T23:45:00Z","nivel":"info","mensaje":"Muestreo completado","contexto":{"cpu":21.4,"memoria":58.2,"disco":71.8}}
```

### Nivel warn — uso elevado de CPU

```json
{"timestamp":"2026-03-07T23:45:10Z","nivel":"warn","mensaje":"Uso de CPU elevado","contexto":{"cpu":91.2,"umbral":90.0}}
```

### Nivel error — fallo de escritura en base de datos

```json
{"timestamp":"2026-03-07T23:45:20Z","nivel":"error","mensaje":"Error al escribir en la base de datos","contexto":{"ruta_db":"ChuspaPulse.db","error":"disk full"}}
```

---

## Integración con agregadores de logs

### Loki (con Promtail)

Configuración mínima de Promtail para recolectar el log de ChuspaPulse:

```yaml
scrape_configs:
  - job_name: ChuspaPulse
    static_configs:
      - targets:
          - localhost
        labels:
          app: ChuspaPulse
          __path__: /var/log/ChuspaPulse.log
    pipeline_stages:
      - json:
          expressions:
            nivel: nivel
            mensaje: mensaje
            timestamp: timestamp
      - labels:
          nivel:
      - timestamp:
          source: timestamp
          format: RFC3339
```

### Filebeat

Configuración mínima de Filebeat para recolectar el log de ChuspaPulse:

```yaml
filebeat.inputs:
  - type: log
    enabled: true
    paths:
      - /var/log/ChuspaPulse.log
    json.keys_under_root: true
    json.add_error_key: true
    json.message_key: mensaje

output.elasticsearch:
  hosts: ["localhost:9200"]
```

---

## Notas

- El log en modo JSON se escribe una línea por evento (NDJSON).
- Si `log_formato` no está definido, ChuspaPulse usa texto plano por defecto.
- El nivel de log se controla con `nivel_log` en `ChuspaPulse.toml`.