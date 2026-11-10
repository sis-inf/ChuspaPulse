# Referencias a la documentación del kernel de Linux

Este documento lista las fuentes de datos y llamadas al sistema que utiliza ChuspaPulse, con su descripción, campos relevantes y enlaces a la documentación oficial.

## Fuentes de datos del sistema de archivos `/proc`

| Ruta / interfaz         | Descripción breve                                                                 | Campos principales que lee ChuspaPulse                  | Enlace a documentación                                                         |
|------------------------|-----------------------------------------------------------------------------------|--------------------------------------------------|--------------------------------------------------------------------------------|
| `/proc/stat`           | Estadísticas globales del procesador y del sistema desde el arranque.             | `cpu`, `intr`, `ctxt`, `btime`                   | https://www.kernel.org/doc/html/latest/filesystems/proc.html#proc-stat         |
| `/proc/meminfo`        | Resumen detallado de la memoria física y de intercambio.                          | `MemTotal`, `MemFree`, `MemAvailable`, `Buffers` | https://www.kernel.org/doc/html/latest/filesystems/proc.html#proc-meminfo      |
| `/proc/net/dev`        | Estadísticas de tráfico de red por interfaz.                                      | `rx_bytes`, `tx_bytes`, `rx_packets`, `tx_packets` | https://www.kernel.org/doc/html/latest/filesystems/proc.html#proc-net-dev      |
| `/proc/uptime`         | Tiempo total de actividad y tiempo de inactividad global del sistema.             | valor total, tiempo inactivo                     | https://www.kernel.org/doc/html/latest/filesystems/proc.html#proc-uptime       |
| `/proc/loadavg`        | Promedio de carga del sistema en 1, 5 y 15 minutos, y número de procesos activos. | promedios, total procesos, último PID            | https://www.kernel.org/doc/html/latest/filesystems/proc.html#proc-loadavg      |

## Llamadas al sistema adicionales

| Llamada al sistema | Descripción                                                                 | Uso en ChuspaPulse                                  | Enlace a documentación                        |
|--------------------|-----------------------------------------------------------------------------|-----------------------------------------------|-----------------------------------------------|
| `statvfs()`        | Obtiene información sobre el sistema de archivos: tamaño, espacio libre, etc. | Cálculo de uso de disco por punto de montaje  | https://man7.org/linux/man-pages/man2/statvfs.2.html |
| `gethostname()`    | Recupera el nombre del host del sistema.                                    | Identificación del agente en los registros    | https://man7.org/linux/man-pages/man2/gethostname.2.html |

---

## Ejemplo de contenido real de `/proc/stat`
cpu  245678 1234 56789 9876543 12345 0 789 0 0 0
cpu0 32100 123 7890 123456 2345 0 123 0 0 0
ctxt  123456789
btime 1718000000

## Ejemplo de contenido real de `/proc/meminfo`
MemTotal:       16234568 kB
MemFree:        8765432 kB
MemAvailable:   11234567 kB
Buffers:         345678 kB
Cached:         2345678 kB
