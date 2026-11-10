# Guía de Collectors

## ¿Qué es un collector?

Un collector es un componente fundamental del sistema de monitoreo de ChuspaPulse encargado de recolectar métricas específicas del sistema operativo. Cada collector se especializa en una categoría particular de métricas, como CPU, memoria, disco, red, carga del sistema, procesos o temperatura, y se encarga de interactuar directamente con las fuentes de datos del kernel Linux para obtener la información en tiempo real.

Los collectors implementan la interfaz `ICollector`, lo que permite que el sistema de muestreo (`sampler`) pueda iterar sobre todos ellos de manera uniforme sin conocer los detalles internos de cada implementación. Esta arquitectura facilita la extensibilidad del sistema, ya que agregar nuevas métricas simplemente implica crear un nuevo collector que siga el contrato establecido por la interfaz y registrarlo en el monitor del sistema.

Cada collector opera de manera independiente y es responsable de manejar sus propios errores de recolección. Cuando el sampler ejecuta un ciclo de muestreo, invoca el método `recolectar()` de cada collector registrado, combina todas las métricas obtenidas en un `Snapshot` consolidado y lo almacena en la base de datos SQLite. Este diseño permite que fallos en un collector no afecten la recolección de métricas de otros componentes, garantizando la robustez del sistema.

## Interfaz ICollector

La interfaz `ICollector` define el contrato que todos los collectors deben implementar. Se encuentra en `src/collectors/icollector.hpp` y contiene los siguientes elementos:

### Campos
No tiene campos de datos, ya que es una interfaz abstracta pura.

### Métodos requeridos

- **`virtual ~ICollector() = default;`**
  Destructor virtual para permitir la destrucción polimórfica correcta a través de punteros a la interfaz base.

- **`virtual std::string nombre() const = 0;`**
  Devuelve el nombre identificador del collector como cadena de texto. Este nombre se utiliza para logging y identificación en caso de errores. Ejemplos: "cpu", "memory", "disk".

- **`virtual std::vector<chuspapulse::core::Metrica> recolectar() = 0;`**
  Ejecuta una medición y devuelve las métricas obtenidas. Retorna un vector de estructuras `chuspapulse::core::Metrica`, donde cada métrica contiene: nombre (string), valor (double), unidad (string) y timestamp (int64_t). Puede lanzar `chuspapulse::core::ErrorRecoleccion` si la medición falla.

## Collectors disponibles

| Nombre | Archivo | Fuente de datos Linux | Métricas que retorna |
|--------|---------|----------------------|---------------------|
| CPU | `src/collectors/cpu/cpu_usage.cpp` | `/proc/stat` | `cpu.usage` (porcentaje) |
| Memoria | `src/collectors/memory/ram_usage.cpp` | `/proc/meminfo` | `ram.total`, `ram.used`, `ram.available` (bytes) |
| Disco | `src/collectors/disk/disk_usage.cpp` | `statvfs()` | `disk.total`, `disk.used`, `disk.free` (bytes) |
| Red | `src/collectors/network/net_usage.cpp` | `/proc/net/dev` | `net.rx_bytes`, `net.tx_bytes` (bytes) |
| Carga promedio | `src/collectors/loadavg/loadavg_collector.cpp` | `/proc/loadavg` | `load.load1`, `load.load5`, `load.load15` |
| Procesos | `src/collectors/procesos/proc_collector.cpp` | `/proc/stat`, `/proc/loadavg` | `procs.running`, `procs.blocked`, `procs.total` |
| Temperatura | `src/collectors/temperatura/temp_collector.cpp` | `/sys/class/thermal/thermal_zone0/temp` | `temp.cpu_celsius`, `temp.zona` |

## Cómo agregar un nuevo collector

Para agregar un nuevo collector al sistema, sigue estos pasos:

1. **Crear los archivos de implementación**
   - Crea un nuevo directorio en `src/collectors/` con el nombre de tu collector (ej: `src/collectors/micollector/`)
   - Crea los archivos `micollector.hpp` y `micollector.cpp` con la implementación

2. **Implementar la interfaz ICollector**
   - En el archivo `.hpp`, crea una clase que herede de `chuspapulse::collectors::ICollector`
   - Implementa el método `nombre()` para retornar el identificador del collector
   - Implementa el método `recolectar()` para leer la fuente de datos correspondiente y retornar un vector de `chuspapulse::core::Metrica`
   - Asegúrate de manejar errores apropiadamente, lanzando `chuspapulse::core::ErrorRecoleccion` cuando sea necesario

3. **Registrar en CMakeLists.txt**
   - Agrega una línea `add_subdirectory(src/collectors/micollector)` en el `CMakeLists.txt` principal
   - Crea un `CMakeLists.txt` en el directorio del collector que defina la biblioteca o añada los fuentes al target principal

4. **Registrar en SystemMonitor**
   - En el código de inicialización del sistema (generalmente en `src/main.cpp` o donde se construye el `SystemMonitor`), crea una instancia de tu collector
   - Regístralo usando el método `SystemMonitor::addCollector(std::make_shared<MiCollector>())`
   - Asegúrate de incluir el header correspondiente en el archivo de inicialización

5. **Probar el collector**
   - Compila el proyecto con `cmake --build build`
   - Ejecuta las pruebas con `ctest --test-dir build`
   - Verifica que las métricas aparezcan en los endpoints `/metrics` y `/metrics/history`

## Fuentes de datos del kernel

Los collectors de ChuspaPulse obtienen información del sistema leyendo archivos virtuales del kernel Linux y llamando a funciones del sistema. Las principales fuentes de datos son:

| Fuente de datos | Descripción | Información proporcionada |
|-----------------|-------------|---------------------------|
| `/proc/stat` | Estadísticas del sistema incluyendo CPU, procesos y context switches | Tiempo de CPU en diferentes modos (user, system, idle, iowait), número de procesos en ejecución y bloqueados, context switches |
| `/proc/meminfo` | Información detallada sobre la memoria del sistema | Memoria total, libre, disponible, buffers, cached, swap total y libre |
| `/proc/net/dev` | Estadísticas de interfaces de red | Bytes recibidos y enviados, paquetes, errores, drops por cada interfaz de red |
| `statvfs()` | Llamada al sistema para estadísticas de sistemas de archivos | Espacio total y libre en bloques, tamaño de bloque, límites de inodos |
| `/sys/class/thermal/thermal_zone0/temp` | Temperatura del CPU en milicelsius | Temperatura actual del CPU en milicelsius (debe dividirse por 1000 para obtener grados Celsius) |

Estas fuentes de datos son estándar en Linux y proporcionan información en tiempo real sin requerir dependencias externas adicionales. Los archivos en `/proc` y `/sys` son sistemas de archivos virtuales que el kernel mantiene dinámicamente, ofreciendo una interfaz unificada para acceder a información del sistema.
