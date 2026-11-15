# Glosario Técnico de Linux y Monitoreo de Infraestructura

| Término | Abreviatura | Definición | Relevancia para ChuspaPulse |
|---|---|---|---|
| `/proc` | — | Pseudo-sistema de archivos que muestra información del kernel y procesos en tiempo real. Permite consultar datos de CPU, memoria, procesos y red. | ChuspaPulse lee `/proc/stat`, `/proc/meminfo`, `/proc/loadavg` y `/proc/net/dev` para recolectar métricas del sistema. |
| `cgroup` | Control Group | Mecanismo de Linux que organiza procesos en grupos y limita o mide los recursos que utilizan. | Permite interpretar el consumo de recursos de servicios agrupados o contenedores. |
| `context switch` | Cambio de contexto | Ocurre cuando el sistema operativo pausa un proceso para ejecutar otro y luego retoma el primero. Muchos cambios pueden consumir tiempo de CPU. | Permite analizar situaciones donde el sistema alterna tareas en lugar de ejecutar trabajo útil. |
| `CPU usage` | Uso de CPU | Porcentaje de tiempo que el procesador dedica a ejecutar tareas activas. Valores altos indican carga intensa o procesos exigentes. | ChuspaPulse calcula el uso de CPU a partir de `/proc/stat`. |
| `daemon` | — | Programa que se ejecuta en segundo plano sin intervención directa del usuario. | ChuspaPulse funciona como agente que recolecta datos continuamente en segundo plano. |
| `file descriptor` | FD | Número que identifica un recurso abierto de un proceso, como un archivo o conexión de red. Cada proceso tiene un límite de descriptores. | Permite detectar procesos que podrían agotar recursos de archivos o conexiones abiertas. |
| `hugepages` | — | Páginas de memoria de gran tamaño que reducen el trabajo administrativo de memoria en aplicaciones grandes. | Permite evaluar rendimiento y comportamiento de memoria de aplicaciones exigentes. |
| `inode` | — | Estructura del sistema de archivos que guarda información sobre archivos o directorios, como permisos y ubicación de datos. | Permite detectar agotamiento de inodes que impediría crear archivos o registros. |
| `I/O wait` | Espera de entrada/salida | Tiempo que la CPU espera a que finalicen operaciones de entrada/salida como lecturas o escrituras en disco. | Permite identificar cuellos de botella en almacenamiento o red. |
| `load average` | Promedio de carga | Promedio de tareas ejecutándose o esperando CPU en intervalos de 1, 5 y 15 minutos. | ChuspaPulse lee `/proc/loadavg` para medir la carga del sistema. |
| `page fault` | Fallo de página | Ocurre cuando un proceso requiere una página de memoria que no está cargada, obligando a traerla de RAM o swap. | Permite identificar presión de memoria o degradación de rendimiento. |
| `RSS` | Resident Set Size | Cantidad de memoria física que un proceso mantiene cargada en RAM. | Permite analizar consumo real de memoria por proceso. |
| `socket` | — | Punto de comunicación que permite intercambio de datos entre procesos en la misma máquina o en red. | Permite medir tráfico de red y comunicaciones de aplicaciones. |
| `swap` | — | Espacio en disco usado como memoria auxiliar cuando la RAM se agota. | Permite detectar presión de memoria y uso de swap. |
| `VSZ` | Virtual Size | Tamaño total de memoria virtual asociada a un proceso, incluyendo memoria usada, reservada o compartida. | Permite analizar memoria virtual solicitada por procesos y aplicaciones. |