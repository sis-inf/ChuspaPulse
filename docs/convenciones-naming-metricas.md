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

## Normalización al exportar a Prometheus

Prometheus solo acepta nombres de métrica que cumplan `[a-zA-Z_:][a-zA-Z0-9_:]*`. Como los nombres internos usan puntos y pueden incluir identificadores de recursos (interfaces, puntos de montaje) con guiones, espacios u otros caracteres, el formatter Prometheus (`FormatterPrometheus::normalizarNombre`) los convierte con esta regla determinista:

1. Se antepone el prefijo `chuspapulse_` (así el primer carácter siempre es válido).
2. Cada carácter que no sea letra ASCII, dígito ni `_` se reemplaza por `_`. Esto incluye `.`, `-`, espacios, `/`, `%`, letras con tilde y también `:`, que Prometheus reserva para reglas de grabación.
3. Las etiquetas (`{core="0"}`) se conservan tal cual; solo se normaliza el nombre.

| Nombre interno | Nombre exportado |
| -------------- | ---------------- |
| `cpu.usage` | `chuspapulse_cpu_usage` |
| `disk.sda-1.used` | `chuspapulse_disk_sda_1_used` |
| `net rx bytes` | `chuspapulse_net_rx_bytes` |
| `uptime_seconds` | `chuspapulse_uptime_seconds` |

### Colisiones

Si en un mismo snapshot dos nombres internos distintos normalizan al mismo nombre (por ejemplo `net.rx` y `net-rx` dan `chuspapulse_net_rx`), no se fusionan en silencio: cada uno recibe el sufijo `_<hash>`, donde `<hash>` son 8 caracteres hexadecimales del hash FNV-1a del nombre interno, y su línea `# HELP` indica el nombre original:

```text
# HELP chuspapulse_net_rx_548b0eb8 Metrica de chuspapulse (nombre original: net.rx)
```

El sufijo depende solo del nombre interno, no del orden en que llegan las métricas, así que el resultado es siempre el mismo. Para evitar sufijos, conviene no crear nombres que solo difieran en caracteres que la normalización reemplaza.
