# Guía de alertas: reglas y umbrales

## Introducción

El sistema de alertas de Pulso notifica cuando una métrica del sistema cumple una condición definida mediante un umbral. Esta guía explica cómo se definen las reglas para CPU, memoria y disco, qué niveles de severidad expone la API y cómo se evitan los falsos positivos.

Componentes principales:

* **AlertaUmbral**: regla que compara una métrica contra un umbral.
* **GestorAlertas**: registra varias reglas, las evalúa contra un `MetricSnapshot` y ejecuta un callback con el mensaje de cada alerta disparada.

---

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

| Métrica    | Descripción                  | Nombres aceptados       |
| ---------- | ---------------------------- | ----------------------- |
| CPU        | Uso de CPU.                  | `"cpu"`, `"cpu.usage_pct"` |
| RAM        | Uso de memoria RAM.          | `"ram"`                 |
| Disco      | Uso de disco.                | `"disk"`                |
| Red (rx)   | Bytes recibidos.             | `"rx_bytes"`            |
| Red (tx)   | Bytes enviados.              | `"tx_bytes"`            |

---

## Registrar y evaluar reglas (`GestorAlertas`)

```cpp
pulso::alertas::GestorAlertas gestor;

gestor.addAlerta(AlertaUmbral("cpu", 90.0f, '>'));
gestor.addAlerta(AlertaUmbral("ram", 85.0f, '>'));
gestor.addAlerta(AlertaUmbral("disk", 95.0f, '>'));

gestor.setCallback([](const std::string& msg) {
    std::cout << "ALERTA: " << msg << std::endl;
});

gestor.evaluar(snapshot);
```

Comportamiento de `evaluar()`:

* Llama al callback solo para las alertas cuya condición se cumple.
* No hace nada si no hay alertas registradas o si no hay callback.
* `cantidadAlertas()` devuelve cuántas reglas hay registradas.

### Dónde se configuran los umbrales

Actualmente los umbrales se registran por código con `GestorAlertas::addAlerta()`. El archivo `pulso.toml` no incluye una sección de alertas, por lo que los umbrales no se pueden cambiar desde ese archivo.

---

## Niveles de severidad

Las alertas que expone la API (`GET /alerts`) incluyen un campo `severity` con uno de estos valores:

| Severidad  | Uso previsto                      |
| ---------- | --------------------------------- |
| `LOW`      | Desviación leve, informativa.     |
| `MEDIUM`   | Requiere seguimiento.             |
| `HIGH`     | Requiere atención pronto.         |
| `CRITICAL` | Riesgo inmediato para el sistema. |

### Ejemplo de respuesta

```json
{
  "alerts": [
    {
      "id": 1,
      "severity": "CRITICAL",
      "message": "Alerta: cpu > 90.000000"
    }
  ]
}
```

Para consultar alertas ya registradas, ver [historial de alertas](historial-alertas.md).

---

## Prevención de falsos positivos

Una lectura aislada por encima del umbral puede ser un pico momentáneo. El caso de uso de monitoreo (ver [casos de uso](casos-de-uso.md)) establece que se genera una alerta cuando el uso de CPU supera el 90 % **durante 3 mediciones consecutivas**.

Ejemplo con umbral `> 90 %` y 3 lecturas consecutivas:

| Lectura | CPU  | ¿Supera 90 %? | Lecturas seguidas | Estado     |
| ------- | ---- | ------------- | ----------------- | ---------- |
| 1       | 92 % | Sí            | 1                 | Sin alerta |
| 2       | 88 % | No            | 0                 | Sin alerta |
| 3       | 93 % | Sí            | 1                 | Sin alerta |
| 4       | 94 % | Sí            | 2                 | Sin alerta |
| 5       | 95 % | Sí            | 3                 | **Alerta** |

Un valor aislado o una oscilación alrededor del umbral no dispara la alerta, porque el contador se reinicia cuando la condición deja de cumplirse.

> **Nota:** actualmente Pulso no aplica un margen de recuperación (histéresis) para resolver las alertas. La resolución depende únicamente de que la condición deje de cumplirse.

---

## Buenas prácticas

* Elegir umbrales acordes al comportamiento normal del sistema.
* Evitar umbrales demasiado bajos para reducir falsas alarmas.
* Considerar el intervalo de muestreo (`intervalo_segundos` en `pulso.toml`): con 10 s, 3 lecturas consecutivas equivalen a unos 30 s sostenidos.
* Revisar periódicamente las alertas activas y el historial.
