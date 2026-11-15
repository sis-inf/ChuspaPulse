# Métricas disponibles en ChuspaPulse

Esta página lista las métricas que genera el agente, con su identificador, descripción, unidad y tipo.

Los tipos siguen el estándar de telemetría:

- **gauge:** valor que puede subir o bajar entre una muestra y otra.
- **counter:** valor acumulado que solo crece mientras el sistema está encendido.

| Identificador de métrica | Descripción | Unidad | Tipo |
|---|---|---|---|
| `cpu.usage` | Porcentaje de uso de la CPU | porcentaje | gauge |
| `cpu.cores` | Núcleos de CPU detectados | cantidad | gauge |
| `memory.used` | Memoria RAM en uso | bytes | gauge |
| `disk.total` | Espacio total del disco | bytes | gauge |
| `disk.used` | Espacio usado del disco | bytes | gauge |
| `disk.free` | Espacio libre del disco | bytes | gauge |
| `net.rx` | Bytes recibidos por red, acumulados desde el arranque | bytes | counter |
| `net.tx` | Bytes enviados por red, acumulados desde el arranque | bytes | counter |
| `procesos.running` | Procesos en ejecución | cantidad | gauge |
| `procesos.blocked` | Procesos bloqueados | cantidad | gauge |
| `procesos.total` | Total de procesos | cantidad | gauge |
| `loadavg.1` | Carga promedio del último minuto | carga | gauge |
| `loadavg.5` | Carga promedio de los últimos 5 minutos | carga | gauge |
| `loadavg.15` | Carga promedio de los últimos 15 minutos | carga | gauge |
| `uptime.seconds` | Tiempo de actividad del sistema | segundos | gauge |
| `temperatura.cpu` | Temperatura de la CPU (solo se reporta si el sensor está disponible) | celsius | gauge |
| `bateria.porcentaje` | Nivel de carga de la batería (solo en equipos con batería) | porcentaje | gauge |
| `bateria.cargando` | Estado de carga de la batería: 1 si está cargando y 0 en cualquier otro caso | booleano | gauge |
