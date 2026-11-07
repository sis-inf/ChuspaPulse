# Guía de configuración del agente ChuspaPulse

Este documento reúne en un solo lugar todas las opciones de configuración del agente ChuspaPulse: las claves del archivo TOML, las variables de entorno equivalentes, sus valores por defecto y un ejemplo completo comentado. Refleja el comportamiento del código de la rama `dev`.

## Cómo se resuelve la configuración

El agente toma cada valor de tres fuentes. Si una opción aparece en varias, gana la de mayor prioridad:

1. **Variables de entorno** `CHUSPAPULSE_*` (mayor prioridad).
2. **Archivo de configuración TOML.**
3. **Valores por defecto** del agente.

Reglas importantes:

- Si el archivo TOML no existe, el agente no falla: usa los valores por defecto y aplica igualmente las variables de entorno.
- Si una clave no está en el archivo, se usa su valor por defecto.
- Si una clave existe pero con un tipo distinto al esperado (por ejemplo, texto donde se espera un número), se ignora y se usa el valor por defecto.
- Cuando el archivo existe, el agente valida los valores finales (ya con las variables de entorno aplicadas) y rechaza los inválidos con un mensaje `Valor inválido para <clave>`. Cuando el archivo no existe, los valores no se validan.

## Ubicación del archivo

| Aspecto | Valor |
|---------|-------|
| Nombre por defecto | `pulso.toml` |
| Ubicación por defecto | Directorio desde el que se ejecuta el agente |
| Ruta alternativa | Opción de línea de comandos `--config <ruta>` |

Ejemplo:

```bash
./ChuspaPulse --config /etc/chuspapulse/produccion.toml
```

El agente no lee una variable de entorno para elegir la ruta del archivo: la única forma de cambiarla es `--config`.

## Referencia de opciones

| Clave TOML | Sección | Tipo | Valor por defecto | Variable de entorno | Valores válidos |
|------------|---------|------|-------------------|---------------------|-----------------|
| `nivel_log` | raíz | texto | `"info"` | `CHUSPAPULSE_NIVEL_LOG` | `debug`, `info`, `warn`, `error` |
| `output_format` | raíz | texto | `"json"` | `CHUSPAPULSE_OUTPUT_FORMAT` | `json`, `csv`, `prometheus` |
| `host` | `[servidor]` | texto | `"0.0.0.0"` | `CHUSPAPULSE_SERVIDOR_HOST` | Dirección IP en la que escucha el servidor |
| `puerto` | `[servidor]` | entero | `8080` | `CHUSPAPULSE_SERVIDOR_PUERTO` | De 1 a 65535 |
| `intervalo_segundos` | `[sampler]` | entero | `10` | `CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS` | Mayor que 0 |
| `ruta_db` | `[storage]` | texto | `"chuspapulse.db"` | `CHUSPAPULSE_STORAGE_RUTA_DB` | Ruta relativa o absoluta |
| `cpu_max` | `[umbrales]` | decimal | `80.0` | No tiene | Umbral de CPU, en porcentaje |
| `ram_max` | `[umbrales]` | decimal | `85.0` | No tiene | Umbral de RAM, en porcentaje |
| `disco_max` | `[umbrales]` | decimal | `90.0` | No tiene | Umbral de disco, en porcentaje |

## Detalle por grupo

### Muestreo (`[sampler]`)

- `intervalo_segundos` define cada cuántos segundos el agente toma una muestra de las métricas. El valor por defecto es 10.
- El archivo de ejemplo del repositorio recomienda valores entre 5 y 60.
- Un valor igual o menor que 0 se rechaza al cargar el archivo.

### Almacenamiento SQLite (`[storage]`)

- `ruta_db` es la ruta del archivo de base de datos SQLite donde el agente guarda los snapshots.
- Puede ser relativa al directorio de ejecución o absoluta.
- El valor por defecto es `chuspapulse.db`, en el directorio de ejecución.

### Servidor HTTP (`[servidor]`)

- `host` es la dirección IP en la que escucha el servidor. Con `0.0.0.0` acepta conexiones desde cualquier interfaz; con `127.0.0.1` solo acepta conexiones locales.
- `puerto` es el puerto TCP del servidor. Debe estar entre 1 y 65535. El valor por defecto es 8080.
- Si el puerto ya está en uso, el servidor no puede iniciar y el agente termina.

### Niveles de logging (`nivel_log`)

| Nivel | Qué se imprime |
|-------|----------------|
| `debug` | Todos los mensajes (`DEBUG`, `INFO`, `WARN` y `ERROR`) |
| `info` | `INFO`, `WARN` y `ERROR` (nivel por defecto) |
| `warn` | `WARN` y `ERROR` |
| `error` | Solo `ERROR` |

Los mensajes por debajo del nivel configurado no se imprimen. Un valor distinto de los cuatro anteriores en el archivo TOML se rechaza al cargar la configuración.

### Umbrales de alerta (`[umbrales]`)

Los tres umbrales (`cpu_max`, `ram_max` y `disco_max`) se expresan en porcentaje y solo se pueden definir en el archivo TOML; no tienen variable de entorno.

### Formato de salida (`output_format`)

Define el formato de salida de las métricas. Los valores válidos son `json`, `csv` y `prometheus`.

## Ejemplo completo comentado

Archivo `pulso.toml` con todas las opciones y sus valores por defecto:

```toml
# Archivo de configuración del agente ChuspaPulse.
# Todas las claves son opcionales: si falta una, se usa su valor por defecto.

# Nivel mínimo de log: debug, info, warn o error.
# Variable de entorno equivalente: CHUSPAPULSE_NIVEL_LOG
nivel_log = "info"

# Formato de salida de las métricas: json, csv o prometheus.
# Variable de entorno equivalente: CHUSPAPULSE_OUTPUT_FORMAT
output_format = "json"

[servidor]
# Dirección IP en la que escucha el servidor HTTP.
# 0.0.0.0 acepta conexiones desde cualquier interfaz.
# Variable de entorno equivalente: CHUSPAPULSE_SERVIDOR_HOST
host = "0.0.0.0"

# Puerto del servidor HTTP (de 1 a 65535).
# Variable de entorno equivalente: CHUSPAPULSE_SERVIDOR_PUERTO
puerto = 8080

[sampler]
# Intervalo entre muestreos consecutivos, en segundos (mayor que 0).
# Variable de entorno equivalente: CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS
intervalo_segundos = 10

[storage]
# Ruta del archivo de base de datos SQLite (relativa o absoluta).
# Variable de entorno equivalente: CHUSPAPULSE_STORAGE_RUTA_DB
ruta_db = "chuspapulse.db"

[umbrales]
# Umbrales de alerta, en porcentaje. No tienen variable de entorno.
cpu_max = 80.0
ram_max = 85.0
disco_max = 90.0
```

## Ejemplos con variables de entorno

Cada variable sobrescribe la clave equivalente del archivo TOML.

En Linux o macOS:

```bash
export CHUSPAPULSE_SERVIDOR_PUERTO=9090
export CHUSPAPULSE_NIVEL_LOG=debug
export CHUSPAPULSE_STORAGE_RUTA_DB=/var/lib/chuspapulse/datos.db
./ChuspaPulse --config pulso.toml
```

En Windows con PowerShell:

```powershell
$env:CHUSPAPULSE_SERVIDOR_PUERTO = "9090"
$env:CHUSPAPULSE_NIVEL_LOG = "debug"
.\ChuspaPulse.exe --config pulso.toml
```

Con el archivo anterior, estas variables hacen que el servidor escuche en el puerto 9090 y que el nivel de log sea `debug`, aunque el archivo diga `puerto = 8080` y `nivel_log = "info"`. El resto de las claves conserva el valor del archivo.

## Notas y limitaciones conocidas

- **Variable `CHUSPAPULSE_CONFIG`:** `docs/variables-entorno.md` la menciona para elegir la ruta del archivo, pero el código no la lee. La ruta solo se cambia con `--config`.
- **Nombre del archivo:** `docs/variables-entorno.md` habla de `ChuspaPulse.toml`; el nombre por defecto que usa el código es `pulso.toml`.
- **Umbrales:** `cpu_max`, `ram_max` y `disco_max` no tienen variable de entorno equivalente.
- **Opciones de línea de comandos:** `--interval`, `--metrics`, `--once` y `--format` son opciones de la línea de comandos y no forman parte del archivo TOML.
