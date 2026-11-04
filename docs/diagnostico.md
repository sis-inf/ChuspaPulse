# Diagnóstico (GET /diagnostico)

El endpoint `GET /diagnostico` permite realizar un autochequeo del estado general de ChuspaPulse. Su respuesta resume el estado de los componentes principales del sistema para facilitar tareas de monitoreo y diagnóstico.

## Estructura de la respuesta

La respuesta se entrega en formato JSON.

```json
{
  "status": "ok",
  "timestamp": 1650000000,
  "version": "0.1.0",
  "components": {
    "storage": {
      "status": "ok",
      "message": ""
    },
    "sampler": {
      "status": "ok",
      "running": true,
      "message": ""
    },
    "collectors": [
      {
        "name": "cpu",
        "status": "ok",
        "message": ""
      },
      {
        "name": "memory",
        "status": "ok",
        "message": ""
      }
    ]
  }
}
```

## Significado de los estados

### `ok`

Indica que el componente funciona correctamente y no se detectaron problemas durante la verificación.

Ejemplos:

- Storage disponible.
- Sampler ejecutándose normalmente.
- Los collectors responden correctamente.

### `degradado`

Indica que el componente continúa funcionando, pero presenta algún comportamiento que puede afectar parcialmente el servicio.

Ejemplos:

- Respuestas lentas.
- Información parcial.
- Algún collector con resultados incompletos.

### `error`

Indica que el componente no puede cumplir correctamente su función y requiere atención.

Ejemplos:

- Base de datos SQLite inaccesible.
- Sampler detenido.
- Un collector produce una excepción durante la recolección.

## Ejemplos

### Sistema saludable

```json
{
  "status": "ok",
  "timestamp": 1650000000,
  "version": "0.1.0",
  "components": {
    "storage": {
      "status": "ok",
      "message": ""
    },
    "sampler": {
      "status": "ok",
      "running": true,
      "message": ""
    },
    "collectors": [
      {
        "name": "cpu",
        "status": "ok",
        "message": ""
      },
      {
        "name": "memory",
        "status": "ok",
        "message": ""
      }
    ]
  }
}
```

### Un collector presenta un error

```json
{
  "status": "error",
  "timestamp": 1650000300,
  "version": "0.1.0",
  "components": {
    "storage": {
      "status": "ok",
      "message": ""
    },
    "sampler": {
      "status": "ok",
      "running": true,
      "message": ""
    },
    "collectors": [
      {
        "name": "cpu",
        "status": "ok",
        "message": ""
      },
      {
        "name": "memory",
        "status": "error",
        "message": "No fue posible obtener la información de memoria."
      }
    ]
  }
}
```

### Storage inaccesible

```json
{
  "status": "error",
  "timestamp": 1650000600,
  "version": "0.1.0",
  "components": {
    "storage": {
      "status": "error",
      "message": "No fue posible acceder a la base de datos."
    },
    "sampler": {
      "status": "degradado",
      "running": true,
      "message": "Los datos no pueden almacenarse."
    },
    "collectors": [
      {
        "name": "cpu",
        "status": "ok",
        "message": ""
      },
      {
        "name": "memory",
        "status": "ok",
        "message": ""
      }
    ]
  }
}
```

## Consultar el endpoint

```bash
curl http://localhost:8080/diagnostico
```

La respuesta permite verificar rápidamente el estado general del servicio y de los componentes principales del sistema.
