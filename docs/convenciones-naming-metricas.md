# Convenciones para el nombrado de métricas

Este documento define la convención utilizada para nombrar las métricas del proyecto ChuspaPulse. Mantener un formato consistente facilita el desarrollo de nuevos collectors y evita inconsistencias entre plataformas y componentes.

## Formato general

Las métricas deben seguir el formato:

```text
categoria.campo
```

Donde:

- `categoria` identifica el componente o recurso monitoreado.
- `campo` identifica la métrica específica.

Las reglas generales son:

- Utilizar únicamente letras minúsculas.
- Utilizar el punto (`.`) como separador.
- Evitar espacios y caracteres especiales.
- Mantener nombres claros y descriptivos.

## Ejemplos actuales

Algunos ejemplos utilizados actualmente en el proyecto son:

- `cpu.usage`
- `cpu.cores`
- `memory.used`

## Convención de sufijos de unidad

Cuando una métrica represente una unidad específica, se recomienda utilizar un sufijo descriptivo para facilitar su interpretación.

Sufijos recomendados:

- `_bytes` para tamaños expresados en bytes.
- `_pct` para porcentajes.
- `_seconds` para tiempos expresados en segundos.

Ejemplos:

- `memory.total_bytes`
- `memory.used_bytes`
- `cpu.usage_pct`
- `uptime_seconds`

## Métricas desglosadas

Cuando una categoría incluya múltiples elementos del mismo tipo, el nombre de la métrica debe incorporar un identificador intermedio para distinguir cada recurso.

Ejemplos:

- `network.<interfaz>.rx_bytes`
- `network.<interfaz>.tx_bytes`
- `disk.<punto_montaje>.total_bytes`
- `disk.<punto_montaje>.used_bytes`

Esta estructura permite representar múltiples interfaces de red, discos u otros recursos manteniendo una convención uniforme y fácil de extender.
