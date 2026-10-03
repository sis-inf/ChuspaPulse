# Guía de Collectors

## 1. Introducción

Un collector (o colector) es el componente encargado de **obtener, leer y organizar** las métricas de uso de recursos del sistema operativo y exponerlas para su procesamiento o visualización. Cada collector se especializa en un tipo de recurso (como procesador, memoria o disco) y funciona de manera independiente sin que otros componentes necesiten saber cómo se lee cada dato.

---

## 2. Collectors implementados

Actualmente se encuentran implementados los siguientes collectors:

| Collector | Clase / Archivo de referencia | Descripción y métricas que devuelve |
| :--- | :--- | :--- |
| **CPU** | `CpuCollector`<br>`src/collectors/cpu.cpp` | Porcentaje de uso total, tiempo de usuario/sistema, número de núcleos y carga del procesador. |
| **RAM** | `RamCollector`<br>`src/collectors/ram.cpp` | Memoria total, en uso, libre, disponible y almacenada en búferes. |
| **Disco** | `DiskCollector`<br>`src/collectors/disk.cpp` | Espacio total, usado, libre, porcentaje de ocupación y operaciones de lectura/escritura. |

---

## 3. Fuentes de datos en Linux

Para la obtención de métricas, los colectores leen directamente las siguientes fuentes del sistema:

| Ruta / Archivo | Información que provee |
| :--- | :--- |
| `/proc/stat` | Estadísticas del procesador, tiempos por modo, interrupciones |
| `/proc/meminfo` | Detalles completos de memoria RAM y áreas reservadas |
| `/proc/net/dev` | Tráfico de red por interfaz: bytes, paquetes, errores |
| `/proc/diskstats` | Actividad de discos: lecturas, escrituras, tiempos de servicio |

---

## 4. Cómo agregar un nuevo collector

1. Define la nueva clase heredando de `BaseCollector` dentro del directorio `src/collectors/`.
2. Agrega la declaración en el archivo de cabecera correspondiente.
3. Implementa el método `collect()` con la lógica necesaria para leer y calcular los valores/métricas requeridas.
4. Registra el nuevo colector en la fábrica o punto de inicialización de la aplicación.
5. Compila el proyecto con CMake para verificar que no haya errores.
6. Agrega pruebas unitarias o de integración si el proyecto las utiliza.
7. Ejecuta la aplicación y comprueba que las métricas del nuevo collector se generen correctamente sin afectar a los demás.
8. Actualiza esta documentación indicando el nombre, ubicación y propósito del nuevo collector.

---

## 5. Buenas prácticas

* Mantener una única responsabilidad por collector.
* Evitar dependencias innecesarias entre collectors.
* Documentar claramente el propósito de cada collector.
* Validar errores y excepciones durante la recolección de métricas.
* Mantener nombres descriptivos y consistentes con el resto del proyecto.