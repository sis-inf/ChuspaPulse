# Guía de Despliegue

## Prerrequisitos

- CMake 3.16 o superior
- Un compilador con soporte de C++17 (por ejemplo, `g++`)
- SQLite (en Linux, el paquete de desarrollo `libsqlite3-dev`)
- Git y conexión a internet durante la configuración: CMake descarga las dependencias (httplib, nlohmann_json, tomlplusplus y SQLiteCpp) con `FetchContent`

## Entornos

### Local

Desde la raíz del repositorio, genera el proyecto y compílalo:

```bash
cmake -B build -S .
cmake --build build
```

El ejecutable queda en `build/bin/chuspapulse`. Ejecútalo indicando el intervalo de lectura en milisegundos:

```bash
./build/bin/chuspapulse --interval 1000
```

Por defecto, el servidor HTTP escucha en el puerto `8080`.

### Producción

En producción, ChuspaPulse se ejecuta como servicio, de modo que se inicie con el equipo y se reinicie si falla. Hay dos opciones.

**Como servicio de systemd.** Los comandos para iniciar, detener, habilitar y consultar los registros del servicio están en [systemd.md](systemd.md).

**Con Docker.** El `Dockerfile` del repositorio compila el binario en modo Release y lo ejecuta con un usuario sin privilegios:

```bash
docker build -t chuspapulse .
docker run -d -p 8080:8080 -v chuspapulse-data:/data chuspapulse
```

Datos que conviene tener en cuenta:

- **Puerto:** el valor por defecto es `8080`, definido en `config.servidor.puerto`.
- **Base de datos:** las métricas se guardan en un archivo SQLite. Por defecto es `chuspapulse.db`, relativo al directorio de ejecución. En Docker queda en `/data`, que es el volumen.
- **Configuración:** el archivo `pulso.toml` del repositorio muestra todas las opciones con sus valores por defecto.

## Variables de entorno

| Variable | Descripción | Ejemplo |
|---|---|---|
| `CHUSPAPULSE_SERVIDOR_HOST` | Dirección en la que escucha el servidor HTTP (por defecto `0.0.0.0`) | `127.0.0.1` |
| `CHUSPAPULSE_SERVIDOR_PUERTO` | Puerto del servidor HTTP (por defecto `8080`) | `9090` |
| `CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS` | Intervalo entre muestreos, en segundos (por defecto `10`) | `30` |
| `CHUSPAPULSE_STORAGE_RUTA_DB` | Ruta del archivo SQLite (por defecto `chuspapulse.db`) | `/data/chuspapulse.db` |
| `CHUSPAPULSE_NIVEL_LOG` | Nivel de log: `debug`, `info`, `warn` o `error` (por defecto `info`) | `debug` |
| `CHUSPAPULSE_OUTPUT_FORMAT` | Formato de salida de métricas: `json`, `csv` o `prometheus` (por defecto `json`) | `prometheus` |

## Solución de problemas comunes

### CMake falla al descargar las dependencias

La configuración descarga las dependencias con `FetchContent`, así que necesita Git y acceso a internet. Instala Git, comprueba la conexión y vuelve a ejecutar `cmake -B build -S .`.

### CMake no encuentra SQLite

Instala el paquete de desarrollo de SQLite (en Debian y Ubuntu, `sudo apt install libsqlite3-dev`) y vuelve a generar el proyecto.

### No se encuentra el ejecutable

El binario se genera en `build/bin/`, no en `build/`. Ejecuta `./build/bin/chuspapulse`.

### Error "Valor inválido para puerto"

El puerto debe estar entre 1 y 65535. Revisa el valor de `servidor.puerto` en el archivo de configuración o de `CHUSPAPULSE_SERVIDOR_PUERTO`.