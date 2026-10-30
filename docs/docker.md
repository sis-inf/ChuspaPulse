# Docker

Este documento describe el estado actual del soporte para Docker en ChuspaPulse y proporciona ejemplos de uso basados en la configuración disponible en el proyecto.

## Estado actual

Actualmente el repositorio no incluye un `Dockerfile`, un archivo `.dockerignore` ni una configuración oficial mediante `docker-compose`.

La aplicación utiliza un archivo de configuración TOML (`chuspa.toml`) y permite indicar una ruta alternativa mediante la opción de línea de comandos:

```bash
./chuspa --config ruta/al/archivo.toml
```

La configuración por defecto es la siguiente:

| Opción | Valor |
| ------- | ----- |
| Host | `0.0.0.0` |
| Puerto | `8080` |
| Base de datos SQLite | `chuspa.db` |

## Construcción de la imagen

Si el proyecto incorpora un `Dockerfile` en la raíz del repositorio, la imagen podrá construirse mediante:

```bash
docker build -t chuspa .
```

## Variables de entorno

Actualmente ChuspaPulse **no implementa variables de entorno** para modificar la configuración de ejecución.

La configuración se realiza mediante:

- El archivo `chuspa.toml`.
- La opción `--config` para indicar un archivo de configuración alternativo.

## Persistencia de la base de datos

Por defecto, la aplicación almacena la base de datos SQLite en el archivo:

```text
chuspa.db
```

Si la aplicación se ejecuta dentro de un contenedor Docker, se recomienda montar un volumen para conservar el archivo `chuspa.db` entre reinicios.

Ejemplo:

```bash
docker run \
  -p 8080:8080 \
  -v $(pwd)/data:/app \
  -v $(pwd)/chuspa.toml:/app/chuspa.toml:ro \
  chuspa \
  --config /app/chuspa.toml
```

En este ejemplo:

- `./data` almacena la base de datos SQLite de forma persistente.
- `chuspa.toml` se monta como un archivo de solo lectura.
- La aplicación utiliza el archivo de configuración indicado mediante `--config`.

## Integración con Prometheus

El proyecto incluye un ejemplo de configuración para Prometheus en:

```text
examples/prometheus.yml
```

Este ejemplo asume que ChuspaPulse escucha en el puerto `8080` y expone las métricas en:

```text
/metrics/prometheus
```

## Resumen

- Actualmente el repositorio no incluye un `Dockerfile` oficial.
- No existen variables de entorno para configurar la aplicación.
- La configuración se realiza mediante `chuspa.toml` y la opción `--config`.
- La base de datos SQLite se almacena en `chuspa.db`.
- Si se utiliza Docker, se recomienda montar un volumen para conservar la base de datos entre reinicios del contenedor.


