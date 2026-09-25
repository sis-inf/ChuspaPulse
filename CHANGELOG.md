# Changelog

Todos los cambios importantes de este proyecto serán documentados en este archivo.

El formato está basado en [Keep a Changelog](https://keepachangelog.com/es/1.1.0/)
y este proyecto sigue el versionado [SemVer](https://semver.org/lang/es/).

## [v0.1.0] - 2026-08-30

### Añadido
- fix(http): activar endpoint GET /config (#833)
- Documentación inicial del proyecto
- Organización inicial de directorios
- Configuración base del sistema
- feat(collector): implementar recolector de métricas de uso de CPU del sistema.
- feat(collector): añadir recolector de estadísticas de consumo de memoria RAM.
- feat(collector): agregar recolector de espacio disponible y uso de almacenamiento en disco.
- feat(collector): implementar recolector de métricas de red para tráfico de interfaces.
- feat(formatter): añadir formateador de salida en formato estructurado JSON.
- feat(formatter): implementar formateador compatible con la exposición de métricas de Prometheus.
- feat(storage): agregar módulo de almacenamiento persistente local para las métricas recolectadas.
- feat(http): implementar handlers HTTP para la exposición de endpoints del servidor de monitoreo.

## [0.1.0] - 2026-04-22

### Añadido
- Inicio del proyecto Pulso
- Creación del repositorio