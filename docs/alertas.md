# Guía de Configuración y Uso del Sistema de Alertas

## Introducción

El sistema de alertas de Pulso permite detectar automáticamente situaciones anómalas en los recursos monitoreados. Su objetivo es notificar cuando una métrica supera un umbral configurado, permitiendo una respuesta rápida ante posibles problemas de rendimiento o disponibilidad.

Los componentes principales son:

* **AlertaUmbral**: representa una regla de alerta basada en un valor límite.
* **GestorAlertas**: evalúa periódicamente las métricas y determina si una alerta debe activarse o resolverse.

---

## Cómo funcionan las alertas

El sistema sigue un ciclo continuo de evaluación:

1. Se recopilan las métricas del sistema.
2. El GestorAlertas obtiene los valores actuales.
3. Cada métrica se compara con su umbral configurado.
4. Si el valor supera el umbral, se genera una alerta activa.
5. Cuando el valor vuelve al rango normal, la alerta se marca como resuelta.
6. Las alertas pueden consultarse mediante la API del sistema.

Este proceso se ejecuta automáticamente durante el monitoreo.

---

## Métricas alertables

| Métrica            | Rango normal | Unidad |
| ------------------ | ------------ | ------ |
| Uso de CPU         | 0 - 80       | %      |
| Uso de RAM         | 0 - 75       | %      |
| Uso de Disco       | 0 - 85       | %      |
| Temperatura de CPU | 30 - 70      | °C     |
| Latencia promedio  | 0 - 200      | ms     |

Los valores anteriores son referenciales y pueden ajustarse según las necesidades del entorno.

---

## Configurar umbrales en pulso.toml

Los umbrales de alerta se configuran mediante el archivo `pulso.toml`.

### Ejemplo de configuración

```toml
[alertas.cpu]
umbral = 90

[alertas.ram]
umbral = 85

[alertas.disco]
umbral = 95
```

### Descripción de parámetros

| Parámetro | Descripción                                          |
| --------- | ---------------------------------------------------- |
| umbral    | Valor límite que activará la alerta.                 |
| cpu       | Configuración de alertas para uso de CPU.            |
| ram       | Configuración de alertas para uso de memoria RAM.    |
| disco     | Configuración de alertas para uso de almacenamiento. |

---

## Consultar alertas activas

Las alertas activas pueden consultarse mediante el siguiente endpoint:

```http
GET /alerts
```

### Ejemplo de respuesta

```json
[
  {
    "metrica": "cpu",
    "valor": 93,
    "umbral": 90,
    "estado": "ACTIVA"
  }
]
```

---

## Ejemplos de alertas comunes

### CPU mayor al 90%

Se activa cuando la utilización del procesador supera el 90%.

### RAM mayor al 85%

Se activa cuando el consumo de memoria supera el 85%.

### Disco mayor al 95%

Se activa cuando el uso del almacenamiento supera el 95%.

---

## Buenas prácticas

* Configurar umbrales acordes al comportamiento esperado del sistema.
* Evitar valores demasiado bajos para reducir falsas alarmas.
* Revisar periódicamente las alertas activas.
* Analizar tendencias históricas antes de modificar umbrales.
* Mantener un equilibrio entre sensibilidad y estabilidad de las alertas.

---

## Resumen

El sistema de alertas de Pulso permite supervisar métricas críticas mediante reglas configurables. Gracias a la evaluación automática de umbrales y la consulta de alertas activas, es posible detectar rápidamente situaciones que requieren atención.

---

## Notificaciones externas vía Webhook

El sistema de alertas permite enviar notificaciones en tiempo real a servicios externos vía peticiones HTTP `POST` con payload JSON cada vez que una alerta se dispara.

### Configuración en `pulso.toml`

Para habilitar el envío automático de notificaciones a un webhook:

```toml
[alertas.webhook]
enabled = true
url = "http://localhost:8080/webhook"