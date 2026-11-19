# Guía de alertas: reglas y umbrales

El módulo `GestorAlertas` evalúa las métricas del sistema en cada ciclo de muestreo. Cuando una alerta cambia de estado, se registra un mensaje de nivel `WARN` en los logs del agente.

El sistema de alertas de ChuspaPulse notifica cuando una métrica del sistema cumple una condición definida mediante un umbral. Esta guía explica cómo se definen las reglas para CPU, memoria y disco, cómo se configuran los umbrales y cómo se comporta el sistema ante lecturas repetidas.

## Componentes principales

* **AlertaUmbral**: regla que compara una métrica contra un umbral, con el operador `>` o `<`. Guarda su propio estado (activa o normal).
* **GestorAlertas**: registra varias reglas, las evalúa contra un `MetricSnapshot`, ejecuta un callback con el mensaje de cada cambio de estado y guarda en la tabla `alerts` las alertas que se activan.

## Cómo funciona

1. El `Sampler` recolecta las métricas de todos los collectors.
2. Guarda el snapshot en la base de datos.
3. Convierte el snapshot a porcentajes de CPU, RAM y disco.
4. `GestorAlertas` evalúa cada alerta contra esos valores.
5. Por cada alerta que cambia de estado, escribe un mensaje de nivel `WARN` en el log. Si la alerta pasa de normal a activa, además guarda un registro en la tabla `alerts` (timestamp, métrica, umbral, valor y mensaje).

El ciclo se repite cada `intervalo_segundos` (sección `[sampler]`).

## Regla de umbral (`AlertaUmbral`)

Cada regla se construye con tres parámetros:

| Parámetro  | Descripción                                         | Ejemplo  |
| ---------- | --------------------------------------------------- | -------- |
| `metrica`  | Nombre de la métrica a evaluar en el snapshot.      | `"cpu"`  |
| `umbral`   | Valor numérico de referencia.                       | `90.0f`  |
| `operador` | Comparación: `'>'` (mayor que) o `'<'` (menor que). | `'>'`    |

La regla está **activa** cuando la métrica cumple la condición.

Errores posibles:

* Si el operador no es `>` ni `<`, el constructor lanza `std::invalid_argument`.
* Si la métrica no es una de las válidas (`cpu`, `cpu.usage_pct`, `ram`, `disk`, `rx_bytes`, `tx_bytes`), `evaluar()` lanza `std::invalid_argument`.

### Métricas disponibles

Provienen de `MetricSnapshot`:

| Métrica    | Descripción                  | Nombres aceptados          |
| ---------- | ---------------------------- | -------------------------- |
| CPU        | Uso de CPU.                  | `"cpu"`, `"cpu.usage_pct"` |
| RAM        | Uso de memoria RAM.          | `"ram"`                    |
| Disco      | Uso de disco.                | `"disk"`                   |
| Red (rx)   | Bytes recibidos.             | `"rx_bytes"`               |
| Red (tx)   | Bytes enviados.              | `"tx_bytes"`               |

## Registrar y evaluar reglas (`GestorAlertas`)

```cpp
chuspapulse::alertas::GestorAlertas gestor;

gestor.addAlerta(AlertaUmbral("cpu", 90.0f, '>'));
gestor.addAlerta(AlertaUmbral("ram", 85.0f, '>'));
gestor.addAlerta(AlertaUmbral("disk", 95.0f, '>'));

gestor.setCallback([](const std::string& msg) {
    std::cout << "ALERTA: " << msg << std::endl;
});

gestor.evaluar(snapshot);
```

Comportamiento de `evaluar()`:

* Cada alerta guarda su propio estado y solo emite eventos cuando ese estado cambia.
* Cambio de normal a activa: llama al callback con el mensaje de la alerta y, si hay base de datos configurada, inserta el registro en la tabla `alerts`.
* Cambio de activa a normal: llama al callback con el mensaje de recuperación (`Alerta resuelta: ...`). Este evento no se guarda en el historial.
* Mientras una alerta sigue activa (o sigue normal) en ciclos consecutivos, no se emite nada.
* No hace nada si no hay alertas registradas. Si no hay callback, solo se omite la notificación.
* `cantidadAlertas()` devuelve cuántas reglas hay registradas.

### Dónde se configuran los umbrales

El agente toma los umbrales de CPU, RAM y disco del archivo de configuración. Son porcentajes (0 a 100) y se definen en la sección `[alertas]`:

```toml
[alertas]
cpu_max = 80.0
ram_max = 85.0
disco_max = 90.0
```

La sección `[umbrales]`, usada antes, sigue aceptándose por compatibilidad. Si una clave aparece en ambas secciones, prevalece el valor de `[alertas]`. Si el archivo o alguna clave no existe, se usan los valores por defecto anteriores (80, 85 y 90).

> Nota: el archivo de configuración por defecto es `chuspapulse.toml`. Si ese archivo no existe pero existe el nombre histórico `pulso.toml` (el archivo de ejemplo que trae el repositorio), se usa este último. Se puede indicar otro archivo con la opción `--config`.

### Ejemplo de salida en el log

Cuando el uso de CPU supera `cpu_max`, aparece una línea de nivel `WARN` con el mensaje de la alerta. Cuando vuelve por debajo del umbral, aparece el mensaje de recuperación:

```
Alerta: cpu > 80.000000
Alerta resuelta: cpu > 80.000000
```

## Consulta por la API

| Endpoint | Descripción |
| -------- | ----------- |
| `GET /alerts` | Alertas activas en este momento, evaluadas contra la última muestra almacenada. |
| `GET /alerts/history` | Historial de alertas activadas, en formato JSON. |

`GET /alerts` devuelve solo las alertas cuya condición se cumple, y cada una incluye `id` (posición de la regla), `message` y `timestamp` (fecha y hora UTC de la última muestra, en formato ISO 8601). Si ninguna se cumple, la lista llega vacía:

```json
{
  "alerts": [
    {
      "id": 1,
      "message": "Alerta: cpu > 90.000000",
      "timestamp": "2026-11-17T14:30:00Z"
    }
  ]
}
```

`/alerts/history` acepta los parámetros opcionales `limite` (entre 1 y 1000, por defecto 100) y `offset` (0 o mayor, por defecto 0). Cada registro incluye `id`, `timestamp`, `metrica`, `umbral`, `valor` y `mensaje`. Con parámetros inválidos responde `400`.

Ejemplo (usa el puerto de `[servidor]`, por defecto 8080):

```bash
curl "http://localhost:8080/alerts/history?limite=10&offset=0"
```

Para más detalle sobre el historial, ver [historial de alertas](historial-alertas.md).

---

## Alertas repetidas y falsos positivos

Una alerta se notifica una sola vez al activarse y otra al resolverse. Mientras la métrica siga por encima del umbral, los ciclos siguientes no repiten el aviso ni el registro en el historial.

El código actual no cuenta lecturas consecutivas: la alerta se activa en la primera lectura que cumple la condición. El caso de uso de monitoreo (ver [casos de uso](casos-de-uso.md)) describe como objetivo exigir 3 mediciones consecutivas, pero eso todavía no está implementado.

> **Nota:** ChuspaPulse no aplica un margen de recuperación (histéresis). La alerta se resuelve en cuanto la condición deja de cumplirse.

---

## Comportamiento y limitaciones

* Las alertas solo se evalúan en modo daemon, no con `--once`.
* Los mensajes del log son de nivel `WARN`: no se ven si `nivel_log` es `"error"`. El historial en la base de datos se guarda igualmente.

## Buenas prácticas

* Elegir umbrales acordes al comportamiento normal del sistema.
* Evitar umbrales demasiado bajos para reducir falsas alarmas.
* Considerar el intervalo de muestreo (`intervalo_segundos` en el archivo de configuración): con un intervalo de 10 s, una condición sostenida se detecta en el ciclo siguiente.
* Revisar periódicamente las alertas activas y el historial.