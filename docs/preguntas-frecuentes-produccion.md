# Preguntas frecuentes sobre operación en producción

Este documento responde a las dudas más comunes de quienes utilizan ChuspaPulse en un entorno de producción. Está orientado a la operación del sistema y no al desarrollo o contribución del código.

---

## ¿Cuánto espacio en disco consume ChuspaPulse con el tiempo?

El espacio utilizado depende principalmente del intervalo de muestreo y del tiempo durante el cual se conservan las métricas. Si el historial crece continuamente, es recomendable definir políticas de limpieza o respaldos periódicos para evitar un crecimiento innecesario del almacenamiento.

---

## ¿Puedo ejecutar varias instancias de ChuspaPulse en el mismo host?

Sí, siempre que cada instancia utilice recursos independientes, como diferentes archivos de almacenamiento o puertos distintos para la API HTTP. Esto evita conflictos entre procesos y facilita la administración de cada instancia.

---

## ¿Cómo actualizo ChuspaPulse sin perder mi historial?

Antes de actualizar la aplicación, se recomienda realizar una copia de seguridad de la base de datos o del almacenamiento utilizado para las métricas. Una vez completada la actualización, restaure dichos datos para conservar el historial previamente recolectado.

---

## ¿El agente afecta el rendimiento del sistema que monitorea?

ChuspaPulse está diseñado para minimizar el consumo de recursos durante la recolección de métricas. No obstante, el impacto puede variar según la frecuencia de muestreo y la cantidad de información almacenada. Para más información consulte `docs/rendimiento.md`.

---

## ¿Es recomendable realizar copias de seguridad del historial?

Sí. Si las métricas recolectadas son importantes para análisis posteriores, es recomendable realizar respaldos periódicos del almacenamiento utilizado por ChuspaPulse. Esto facilita la recuperación de la información ante fallos del sistema o durante procesos de actualización.

---

## ¿Este documento reemplaza las preguntas frecuentes para contribuidores?

No. Este documento está dirigido exclusivamente a la operación de ChuspaPulse en producción. Para consultas relacionadas con la compilación, desarrollo o contribuciones al proyecto, consulte `docs/preguntas-frecuentes.md`.
