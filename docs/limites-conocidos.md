# Limitaciones conocidas

Este documento lista las limitaciones conocidas de ChuspaPulse a la fecha (rama `dev`). Su objetivo es aclarar qué capacidades no deben asumirse sin una validación adicional.

---

## Alcance

Incluye limitaciones relacionadas con almacenamiento histórico, recolección de métricas entre plataformas, diferencias de precisión entre sistemas operativos, soporte en macOS y configuración de interfaces de red.

No describe mejoras futuras ni propuestas de desarrollo; únicamente documenta limitaciones observadas en la implementación actual.

---

## Resumen ejecutivo

- **Storage:** ChuspaPulse utiliza SQLite para almacenar métricas históricas. Es adecuado para almacenamiento local, pero no está diseñado para escenarios multi-host ni replicación distribuida.
- **Load average:** la métrica depende de la implementación del sistema operativo. Los valores obtenidos en Linux y Windows no tienen necesariamente la misma semántica y no deben compararse directamente.
- **macOS:** actualmente no está soportado. Los colectores nativos para macOS 12+ están pendientes.
- **Interfaces de red:** actualmente no existe una configuración para seleccionar manualmente interfaces específicas de monitoreo.

---

## Limitaciones detalladas

### Storage (SQLite)

ChuspaPulse utiliza SQLite como almacenamiento local de métricas históricas.

El modo WAL (`Write-Ahead Log`) permite mejorar la concurrencia dentro del mismo entorno local, pero no convierte SQLite en una base de datos distribuida.

Limitaciones:

- SQLite no proporciona replicación ni alta disponibilidad multi-host.
- No se debe asumir un funcionamiento adecuado para múltiples hosts escribiendo sobre el mismo archivo de base de datos.
- Para grandes volúmenes históricos o despliegues distribuidos puede ser necesario utilizar otro sistema de almacenamiento.

Referencias:

- `src/storage/storage.cpp`
- `src/storage/storage.hpp`

---

## Load average: diferencias entre Linux y Windows

En Linux, ChuspaPulse obtiene la métrica load average mediante fuentes del sistema como `/proc/loadavg`.

Esta métrica representa los promedios de carga del sistema a 1, 5 y 15 minutos.

Limitaciones:

- Windows no proporciona `/proc/loadavg` ni una métrica equivalente con la misma semántica.
- Cualquier aproximación implementada en otro sistema operativo puede representar una métrica diferente.
- Los valores de load average entre Linux y Windows no deben interpretarse como equivalentes directos.

Referencia:

- `src/collectors/loadavg/loadavg_collector.cpp`

---

## Soporte en macOS

macOS 12+ no está soportado actualmente. Los colectores nativos para esa plataforma están pendientes, por lo que ChuspaPulse no debe desplegarse en macOS hasta que exista soporte oficial.

Las plataformas soportadas actualmente son Linux (Kernel 5.4+) y Windows 10/11.

---

## Interfaces de red

Los collectors de red utilizan las interfaces disponibles proporcionadas por el sistema operativo.

Limitaciones:

- Actualmente no existe una opción de configuración para seleccionar o excluir interfaces específicas.
- En equipos con múltiples interfaces (por ejemplo VLAN, bridges o interfaces virtuales), pueden recopilarse datos de todas las interfaces visibles para el sistema.

Referencias:

- `docs/collectors.md`
- `docs/colectores.md`

---

## Consideraciones finales

Las limitaciones descritas corresponden al estado actual del proyecto en la rama `dev`.

Estas restricciones deben considerarse al desplegar ChuspaPulse en entornos con requisitos de alta disponibilidad, múltiples sistemas operativos o configuraciones avanzadas de red.