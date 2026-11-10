# Historial de alertas

## Introducción

El historial de alertas permite consultar las alertas que fueron disparadas por el sistema y registradas de forma persistente en SQLite. A diferencia del endpoint `GET /alerts`, que muestra únicamente las alertas activas en el momento de la consulta, el endpoint `GET /alerts/history` recupera las alertas registradas anteriormente, con paginación básica.

Este documento describe la estructura de la tabla `alerts`, los parámetros disponibles para consultar el historial y el formato de respuesta de la API.

---

## Tabla `alerts`

Cada vez que `GestorAlertas` evalúa una métrica y la condición de una alerta se cumple, se inserta un registro en la tabla `alerts`. La tabla se crea automáticamente al iniciar el servicio.

### Estructura

| Campo | Descripción |
|--------|-------------|
| `id` | Identificador único del registro (autoincremental). |
| `timestamp` | Marca de tiempo Unix, en segundos, del momento en que se registró la alerta. |
| `metrica` | Nombre de la métrica que originó la alerta. |
| `umbral` | Valor configurado como límite para la alerta. |
| `valor` | Valor observado de la métrica cuando la alerta fue disparada. |
| `mensaje` | Mensaje descriptivo asociado a la alerta. |

---

## Consultar el historial de alertas

```http
GET /alerts/history
```

Las alertas se devuelven ordenadas por `timestamp`, de la más reciente a la más antigua.

### Parámetros opcionales

| Parámetro | Descripción |
|-----------|-------------|
| `limite` | Cantidad máxima de alertas a devolver. Entero entre 1 y 1000. Por defecto: 100. |
| `offset` | Cantidad de alertas a omitir, para paginar. Entero mayor o igual a 0. Por defecto: 0. |

Si algún parámetro no es válido, el endpoint responde con código 400 y el cuerpo `{"error":"parametros invalidos"}`.

---

## Ejemplo de consulta

```bash
curl "http://localhost:8080/alerts/history?limite=10&offset=0"
```

---

## Ejemplo de respuesta JSON

La respuesta es un arreglo JSON con las alertas registradas:

```json
[
  {
    "id": 2,
    "timestamp": 1719901480,
    "metrica": "ram",
    "umbral": 85.0,
    "valor": 87.1,
    "mensaje": "Alerta: ram > 85.000000"
  },
  {
    "id": 1,
    "timestamp": 1719900123,
    "metrica": "cpu",
    "umbral": 90.0,
    "valor": 94.8,
    "mensaje": "Alerta: cpu > 90.000000"
  }
]
```

---

## Consideraciones

- El historial permite consultar eventos registrados previamente, incluso si la alerta ya no se encuentra activa.
- Se registra una fila en cada ciclo de muestreo en el que la condición de la alerta se cumple, por lo que una alerta sostenida genera varios registros.
- Si no hay alertas registradas, la respuesta es un arreglo vacío (`[]`).
- La respuesta utiliza formato JSON, igual que el resto de los endpoints HTTP del proyecto.