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

## Intervalo de lectura (`--interval`)

La opción de línea de comandos `--interval` define cada cuánto se leen las métricas.

- **Unidad:** milisegundos.
- **Rango permitido:** de `101` a `3600000` ms (de algo más de 100 ms a 1 hora), ambos incluidos.
- **Valor por defecto:** `1000` ms.

Solo se aceptan enteros escritos con dígitos. Si el valor no es un entero positivo (por ejemplo `abc`, `1.5`, `-5` o `100abc`) o queda fuera del rango (por ejemplo `0`, `100` o `3600001`), el programa muestra un mensaje de error y termina con código de salida distinto de cero:

```text
Error: --interval debe ser un entero positivo en milisegundos (recibido: 'abc')
Error: --interval fuera de rango: 0 ms (permitido: 101 a 3600000 ms)
```

Ejemplo:

```bash
./build/bin/chuspapulse --interval 2000
```

## Prioridad de configuración

El orden de prioridad es el siguiente:

1. Opción `--config` proporcionada en la línea de comandos.
2. Variable de entorno `CHUSPAPULSE_CONFIG`.
3. Archivo de configuración `ChuspaPulse.toml`.
4. Valores por defecto definidos por la aplicación.
