# Auto-monitoreo del Agente

## Descripción

Además de recopilar métricas del sistema monitoreado, **ChuspaPulse** realiza un seguimiento de su propio estado de funcionamiento mediante un conjunto de métricas de auto-monitoreo.

Estas métricas permiten verificar que el agente se ejecuta correctamente, identificar consumos excesivos de recursos y detectar degradaciones de rendimiento antes de que afecten la recolección de información.

Es importante destacar que estas métricas describen el comportamiento del propio agente **ChuspaPulse**, no del equipo o sistema que está siendo monitoreado.

---

# Métricas disponibles

## `chuspapulse.self_memory_bytes`

### Descripción

Indica la cantidad de memoria utilizada por el proceso de ChuspaPulse.

### Unidad

Bytes.

### Propósito

Permite detectar incrementos anormales en el consumo de memoria, como posibles fugas (memory leaks) o un crecimiento inesperado durante la ejecución prolongada del agente.

### Interpretación

* Valores estables indican un consumo de memoria normal.
* Un crecimiento continuo puede requerir una revisión del comportamiento del agente.

---

## `chuspapulse.self_cpu_pct`

### Descripción

Representa el porcentaje de uso de CPU correspondiente únicamente al proceso de ChuspaPulse.

### Unidad

Porcentaje (%).

### Propósito

Permite verificar que el agente no consuma una cantidad excesiva de recursos del procesador mientras realiza las tareas de monitoreo.

### Interpretación

* Un uso bajo suele ser el comportamiento esperado.
* Un incremento sostenido puede indicar ciclos de trabajo demasiado frecuentes o procesamiento excesivo.

---

## `chuspapulse.collector_duration_ms`

### Descripción

Mide el tiempo necesario para ejecutar un collector individual.

### Unidad

Milisegundos (ms).

### Propósito

Permite identificar collectors cuyo tiempo de ejecución es superior al esperado.

### Interpretación

* Duraciones bajas indican una ejecución eficiente.
* Duraciones elevadas pueden señalar operaciones costosas o bloqueos durante la recolección de datos.

---

## `chuspapulse.ciclo_duration_ms`

### Descripción

Indica el tiempo total requerido para completar un ciclo completo de recolección de métricas.

### Unidad

Milisegundos (ms).

### Propósito

Permite conocer el rendimiento global del proceso de monitoreo y verificar si el tiempo de ejecución del ciclo permanece dentro de los límites esperados.

### Interpretación

* Un tiempo estable facilita mantener una frecuencia constante de monitoreo.
* Un incremento progresivo puede afectar la periodicidad de la recolección.

---

# Importancia del auto-monitoreo

Las métricas de auto-monitoreo ayudan a evaluar la salud del propio agente ChuspaPulse.

Gracias a ellas es posible detectar situaciones como:

* Consumo excesivo de memoria.
* Uso elevado de CPU.
* Collectors con tiempos de ejecución elevados.
* Ciclos completos de monitoreo demasiado lentos.

Esta información permite identificar problemas del agente antes de que afecten la calidad de las métricas recolectadas.

---

# Diferencia respecto a las métricas del sistema

Las métricas de auto-monitoreo no describen el estado del sistema monitoreado.

Su objetivo es medir exclusivamente el comportamiento interno del proceso **ChuspaPulse**, permitiendo supervisar el rendimiento y la estabilidad del agente encargado de recopilar las métricas.
