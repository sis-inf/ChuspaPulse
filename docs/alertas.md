# Sistema de Alertas

El módulo `GestorAlertas` evalúa las métricas del sistema en cada ciclo de muestreo. Si una métrica supera su umbral, se registra una advertencia (`WARN`) en los logs del agente.

## Componentes

* **AlertaUmbral**: regla que compara una métrica contra un valor límite con el operador `>` o `<`.
* **GestorAlertas**: guarda varias `AlertaUmbral`, las evalúa todas contra un snapshot y ejecuta un callback por cada alerta que se dispara.

## Cómo funciona

1. El `Sampler` recolecta las métricas de todos los collectors.
2. Guarda el snapshot en la base de datos.
3. Convierte el snapshot a porcentajes de CPU, RAM y disco.
4. `GestorAlertas` evalúa cada alerta contra esos valores.
5. Por cada alerta disparada, escribe un mensaje de nivel `WARN` en el log.

El ciclo se repite cada `intervalo_segundos` (sección `[sampler]`).

## Métricas evaluadas

| Métrica | Origen del valor | Operador |
| ------- | ---------------- | -------- |
| CPU     | `cpu.usage` (porcentaje) | `>` |
| RAM     | `memoria usada / memoria total * 100` | `>` |
| Disco   | `disk.used / disk.total * 100` | `>` |

## Validación de reglas

Cada `AlertaUmbral` se valida al crearla, no al evaluarla. Si la métrica o el operador no son válidos, el constructor lanza `std::invalid_argument` con un mensaje que indica el dato inválido, y la regla no llega a guardarse.

* **Métricas válidas:** `cpu`, `cpu.usage_pct`, `ram`, `disk`, `rx_bytes`, `tx_bytes`.
* **Operadores válidos:** `>` y `<`.

Ejemplos de error:

```
Métrica desconocida: 'temperatura'. Métricas válidas: cpu, cpu.usage_pct, ram, disk, rx_bytes, tx_bytes.
Operador no válido: '='. Use '>' o '<'.
```

## Configuración de umbrales (`pulso.toml`)

Los umbrales se leen desde la sección `[umbrales]`. Todos son porcentajes (0 a 100):

```toml
[umbrales]
cpu_max = 80.0
ram_max = 85.0
disco_max = 90.0
```

Si el archivo o alguna clave no existe, se usan esos mismos valores por defecto.

## Ejemplo de salida en el log

Cuando el uso de CPU supera `cpu_max`, aparece una línea de nivel `WARN` con el mensaje de la alerta, por ejemplo:

```
Alerta: cpu > 80.000000
```

## Comportamiento y limitaciones

* Las alertas solo se evalúan en modo daemon, no con `--once`.
* Mientras la métrica siga por encima del umbral, la advertencia se repite en cada ciclo. No existe un estado de "alerta resuelta".
* Los mensajes son de nivel `WARN`: no se ven si `nivel_log` es `"error"`.
* Las alertas no se consultan por la API. El endpoint `/alerts` está fuera del alcance de esta integración.