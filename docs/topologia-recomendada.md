# Topologías recomendadas para desplegar ChuspaPulse

## Resumen

ChuspaPulse puede utilizarse en diferentes escenarios de monitoreo, desde un único servidor hasta entornos con múltiples equipos. La configuración recomendada depende del tamaño de la infraestructura y de los requisitos de almacenamiento y consulta de métricas.

Este documento presenta recomendaciones para tres escenarios habituales de despliegue.

---

# Escenario 1: Un único servidor

## Cuándo utilizarlo

Este escenario es adecuado cuando ChuspaPulse monitoriza un solo equipo, por ejemplo:

- Un servidor personal.
- Un laboratorio.
- Un servidor de desarrollo.
- Un equipo de pruebas.

## Configuración recomendada

Se recomienda utilizar una configuración sencilla:

- Un único proceso de ChuspaPulse.
- Almacenamiento local.
- Retención de métricas según las necesidades del usuario.
- Consultas realizadas directamente sobre la instancia.

Esta configuración ofrece la menor complejidad administrativa y resulta suficiente para la mayoría de instalaciones pequeñas.

---

# Escenario 2: Pequeño grupo de servidores (2 a 10)

## Cuándo utilizarlo

Este escenario es apropiado cuando se desea monitorizar varios servidores de forma coordinada, por ejemplo:

- Laboratorios institucionales.
- Pequeñas empresas.
- Ambientes de desarrollo.
- Clústeres de tamaño reducido.

## Recomendaciones

Para facilitar el análisis de las métricas se recomienda:

- Identificar correctamente cada servidor mediante su hostname.
- Filtrar las consultas utilizando dicho identificador.
- Configurar una política de retención adecuada para evitar el crecimiento innecesario de la base de datos.

Este enfoque permite mantener una administración sencilla mientras el número de equipos permanece reducido.

## Limitaciones

ChuspaPulse utiliza SQLite como mecanismo de almacenamiento local.

Aunque SQLite resulta adecuado para este tipo de despliegues, su utilización presenta limitaciones cuando aumenta el volumen de datos o el número de escrituras concurrentes.

Estas limitaciones se describen con mayor detalle en el documento:

- `docs/limites-conocidos.md`

---

# Escenario 3: Flotas grandes

## Cuándo utilizarlo

Este escenario corresponde a infraestructuras con decenas o cientos de servidores.

En estos casos no se recomienda utilizar ChuspaPulse como agregador central de todas las métricas.

## Recomendación

La práctica recomendada consiste en ejecutar una instancia de ChuspaPulse en cada servidor y exportar las métricas hacia una plataforma especializada en agregación y almacenamiento.

Algunas opciones habituales son:

- Prometheus
- VictoriaMetrics
- InfluxDB

ChuspaPulse puede actuar como generador de métricas utilizando formatos compatibles como:

- Prometheus
- OpenMetrics
- Influx

Las herramientas especializadas permiten:

- Centralizar métricas.
- Consultar grandes volúmenes de información.
- Generar alertas complejas.
- Escalar horizontalmente.

---

# Comparación de escenarios

| Escenario | Configuración recomendada | Observaciones |
|-----------|---------------------------|---------------|
| Servidor único | Una instancia de ChuspaPulse con almacenamiento local | Configuración sencilla y de bajo mantenimiento. |
| 2 a 10 servidores | Uso de hostname para identificar equipos y consultas filtradas | Considerar las limitaciones de SQLite descritas en `docs/limites-conocidos.md`. |
| Flotas grandes | Una instancia por servidor y exportación hacia Prometheus, VictoriaMetrics o InfluxDB | No utilizar ChuspaPulse como agregador central. |

---

# Buenas prácticas

Independientemente del tamaño del despliegue, se recomienda:

- Configurar una retención de datos acorde con el espacio disponible.
- Supervisar periódicamente el crecimiento del almacenamiento.
- Utilizar nombres de host descriptivos.
- Integrar ChuspaPulse con plataformas de monitoreo especializadas cuando el entorno aumente de tamaño.

---

# Conclusiones

ChuspaPulse puede utilizarse eficazmente en distintos escenarios de monitoreo siempre que la configuración sea acorde al tamaño del entorno.

Para instalaciones pequeñas resulta suficiente una configuración local. En entornos medianos es recomendable organizar las consultas mediante hostnames y considerar las limitaciones conocidas del almacenamiento. Para flotas grandes, la práctica recomendada consiste en utilizar ChuspaPulse como agente de recolección y delegar la agregación de métricas en soluciones especializadas como Prometheus, VictoriaMetrics o InfluxDB.