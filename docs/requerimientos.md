# Requerimientos del Proyecto

## Requerimientos Funcionales

Un requerimiento funcional define las funciones específicas que un sistema debe realizar para cumplir con las expectativas de los usuarios y del negocio. Son esenciales en proyectos de software porque permiten planificar, desarrollar y evaluar el producto de manera ordenada y verificable.

| ID | Descripción | Prioridad | Estado |
|---|---|---|---|
| RF-001 | El sistema debe monitorear la disponibilidad de servidores y servicios básicos mediante ping y chequeos HTTP.| Alta | Pendiente |
| RF-002 | El sistema debe recolectar y mostrar métricas esenciales de CPU, memoria y red en tiempo real. | Alta | Pendiente
| RF-003 | El sistema debe generar alertas inmediatas cuando un servicio caiga o supere umbrales críticos definidos. | Alta | Pendiente
| RF-004 | El sistema debe ofrecer un panel web simple para visualizar el estado de la infraestructura. | Alta | Pendiente
| RF-005 | El sistema debe almacenar métricas históricas por al menos 24 horas para análisis básico de tendencias. | Media | Pendiente
| RF-006 | El sistema debe permitir la instalación y configuración sencilla de agentes en servidores monitoreados. | Alta | Pendiente
| RF-007 | El sistema debe permitir definir umbrales personalizados para CPU, memoria y red. | Media | Pendiente
| RF-008 | El sistema debe exportar reportes básicos de estado y métricas en formatos estándar (CSV o PDF). | Baja | Pendiente

## Requerimientos No Funcionales (RNF)

Los siguientes requerimientos definen las restricciones técnicas y de calidad bajo las cuales debe operar el agente **Pulso**.

| ID | Categoría | Descripción | Prioridad |
|:---:|---|---|:---:|
| **RNF-001** | **Rendimiento** | El agente debe consumir menos de **10MB de memoria RAM** durante su ejecución continua. | Alta |
| **RNF-002** | **Rendimiento** | El tiempo de respuesta para la captura de métricas de sistema debe ser inferior a **100ms**. | Alta |
| **RNF-003** | **Compatibilidad** | El agente debe ser compatible actualmente con **Linux (Kernel 5.4+) y Windows 10/11**; el soporte para **macOS 12+** queda pendiente. | Alta |
| **RNF-004** | **Disponibilidad** | El agente debe ser capaz de reintentar la conexión automáticamente en caso de pérdida de red (Resiliencia). | Media |
| **RNF-005** | **Disponibilidad** | El sistema debe garantizar un tiempo de actividad (Uptime) del 99.9% para no perder métricas críticas. | Media |
| **RNF-006** | **Seguridad** | Los datos transmitidos entre el agente y el servidor deben estar cifrados mediante TLS 1.2 o superior. | Media |
| **RNF-007** | **Escalabilidad** | El agente debe poder ejecutarse en arquitecturas de procesador x86_64 y ARM64. | Baja |
| **RNF-008** | **Rendimiento** | El sistema debe responder en menos de **500ms** al consultar métricas de CPU y memoria en tablas de hasta 10,000 registros. | Alta |
| **RNF-009** | **Portabilidad** | El agente debe mantener compatibilidad con **Windows 10/11 y Linux Kernel 5.4+**. El soporte para **macOS 12+** queda como trabajo futuro. | Media |
| **RNF-010** | **Seguridad** | El sistema debe acceder a métricas de bajo nivel (ej. `/proc`) únicamente con permisos adecuados y cifrar la transmisión con **TLS 1.2+**. | Alta |
| **RNF-011** | **Mantenibilidad** | La cobertura de pruebas unitarias debe ser al menos del **80%** en cada release. | Media |

## Requerimientos de Sistema

| ID | Descripción |
|---|---|
| RS-001 | Hardware mínimo: procesador de 2 núcleos, 4 GB de memoria RAM y 200 MB de espacio libre en disco para logs y métricas. |
| RS-002 | Sistemas operativos compatibles actualmente: **Linux Kernel 5.4+** y **Windows 10/11**. **macOS 12+** aún no está soportado. |
| RS-003 | Versión mínima de compilador: **GCC 11+** en Linux y **MSVC 2019+** en Windows. |
| RS-004 | Resolución mínima de pantalla: **1280x720** para visualización correcta del dashboard web. |
| RS-005 | El sistema debe tener acceso a puertos estándar de red (ej. 5432 para PostgreSQL, 80/443 para HTTP/HTTPS). |
| RS-006 | El instalador completo no debe exceder los **100 MB** de almacenamiento. |

---

## Requerimientos adicionales de rendimiento

Los siguientes requisitos definen los límites de operación del sistema:

| ID       | Categoría     | Descripción                                                                 | Prioridad |
|----------|---------------|-----------------------------------------------------------------------------|-----------|
| **RNF-012** | Rendimiento   | Procesar **mínimo 100 métricas por segundo** sin pérdida de datos.          | Alta      |
| **RNF-013** | Rendimiento   | Respuesta del endpoint `/metrics` en **menos de 200 ms** bajo carga normal. | Alta      |
| **RNF-014** | Rendimiento   | Consumo de CPU **máximo 70%** en estado estable.                           | Alta      |
| **RNF-015** | Rendimiento   | Consumo de RAM **máximo 200 MB** en operación normal.                      | Media     |
| **RNF-016** | Disponibilidad| Disponibilidad del servicio **99.9% mensual**.                             | Alta      |
| **RNF-017** | Escalabilidad | Soportar aumento de carga **hasta el doble** sin modificaciones mayores.    | Baja      |