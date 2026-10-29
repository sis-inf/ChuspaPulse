# Variables de entorno

## Introducción

ChuspaPulse permite configurar su comportamiento mediante un archivo de configuración en formato TOML. Además, la especificación del proyecto contempla el uso de variables de entorno con prefijo `CHUSPAPULSE_` para sobrescribir valores de configuración.

La prioridad de resolución de la configuración es la siguiente:

1. Variables de entorno (`CHUSPAPULSE_*`)
2. Archivo de configuración TOML
3. Valores por defecto definidos por la aplicación

## Variables documentadas

| Variable | Campo de configuración | Tipo esperado | Prioridad |
|----------|-------------------------|---------------|-----------|
| `CHUSPAPULSE_CONFIG` | Ruta del archivo de configuración | `string` | `--config` > `CHUSPAPULSE_CONFIG` > `ChuspaPulse.toml` |

## Ejemplo de uso

Para utilizar un archivo de configuración distinto al predeterminado:

```bash
export CHUSPAPULSE_CONFIG="/etc/ChuspaPulse/produccion.toml"
./build/bin/ChuspaPulse
```

También es posible utilizar un archivo TOML con una configuración como la siguiente:

```toml
[servidor]
host = "0.0.0.0"
puerto = 8080

[sampler]
intervalo_segundos = 10

[storage]
ruta_db = "ChuspaPulse.db"

nivel_log = "info"
output_format = "json"
```

## Prioridad de configuración

El orden de prioridad es el siguiente:

1. Opción `--config` proporcionada en la línea de comandos.
2. Variable de entorno `CHUSPAPULSE_CONFIG`.
3. Archivo de configuración `ChuspaPulse.toml`.
4. Valores por defecto definidos por la aplicación.
