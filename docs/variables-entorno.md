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
| `CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS` | `[sampler] intervalo_segundos` | `int` (segundos) | `--interval` > `CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS` > `intervalo_segundos` del TOML > 10 s |

## Intervalo de muestreo

El intervalo con el que el sampler recolecta métricas puede definirse en tres
lugares. El valor efectivo se resuelve en este orden:

| Prioridad | Origen | Unidad | Rango válido |
|-----------|--------|--------|--------------|
| 1 | Opción `--interval <ms>` | Milisegundos | Entero mayor a 100 |
| 2 | Variable `CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS` | Segundos | Entero mayor a 0 |
| 3 | `intervalo_segundos` en la sección `[sampler]` del TOML | Segundos | Entero mayor a 0 |
| 4 | Valor por defecto | Segundos | 10 |

Si se pasa `--interval`, ese valor prevalece sobre la configuración. Si no se
pasa, se usa el valor de la configuración convertido a milisegundos. Un
`--interval` menor o igual a 100 se rechaza con un mensaje de error y código de
salida distinto de cero.

```bash
# Muestrear cada 500 ms aunque el TOML indique otro intervalo
./build/bin/ChuspaPulse --interval 500
```

Al iniciar, el agente registra en el log el intervalo efectivo, por ejemplo
`Intervalo de muestreo: 500 ms`.

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
