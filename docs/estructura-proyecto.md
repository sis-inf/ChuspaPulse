# Estructura del proyecto ChuspaPulse

## Introducción

Este documento describe la estructura de directorios del proyecto ChuspaPulse y explica el propósito de cada carpeta principal. Su objetivo es facilitar la incorporación de nuevos colaboradores y servir como guía para ubicar correctamente el código fuente, pruebas y documentación.

---

## Árbol de directorios

```text
ChuspaPulse/
├── .github/
│   ├── ISSUE_TEMPLATE/
│   ├── workflows/
│   ├── dependabot.yml
│   └── PULL_REQUEST_TEMPLATE.md
│
├── cmake/
│   └── Dependencies.cmake
│
├── docs/
│   └── img/
│
├── src/
│   ├── alertas/
│   ├── cli/
│   ├── collectors/
│   │   ├── cpu/
│   │   ├── disk/
│   │   ├── loadavg/
│   │   ├── memory/
│   │   ├── network/
│   │   ├── procesos/
│   │   ├── temperatura/
│   │   └── uptime/
│   ├── config/
│   ├── core/
│   ├── formatters/
│   ├── http/
│   ├── platform/
│   │   ├── linux/
│   │   └── windows/
│   ├── sampler/
│   ├── storage/
│   └── utils/
│       └── logging/
│
├── tests/
│   ├── automatizados/
│   │   ├── e2e/
│   │   ├── integration/
│   │   └── unit/
│   ├── casos/
│   │   ├── funcionales/
│   │   ├── no-funcionales/
│   │   │   ├── rendimiento/
│   │   │   └── seguridad/
│   │   └── regresion/
│   ├── core/
│   ├── datos/
│   │   ├── entrada/
│   │   └── esperados/
│   ├── fixtures/
│   ├── formatters/
│   ├── manuales/
│   │   ├── checklists/
│   │   └── evidencias/
│   ├── plan/
│   ├── reportes/
│   └── storage/
│
├── examples/
├── scripts/
├── security/
├── data/
├── CMakeLists.txt
├── Makefile
├── README.md
└── ChuspaPulse.toml
```

---

## Directorio src/

Contiene el código fuente principal de la aplicación.

### src/alertas/

Implementa el sistema de alertas basado en umbrales y reglas de monitoreo.

Contiene clases relacionadas con la evaluación y gestión de alertas.

### src/cli/

Implementa el procesamiento de argumentos de línea de comandos y opciones de ejecución.

### src/collectors/

Contiene los módulos encargados de recolectar métricas del sistema operativo.

Archivos destacados:

* CPU (`cpu_usage.cpp`, `cpu_usage.hpp`)
* Memoria RAM (`ram_usage.cpp`, `ram_usage.h`)
* Disco (`disk_usage.cpp`, `disk_usage.h`)
* Red (`network_io.cpp`, `net_usage.cpp`)
* Procesos (`proc_collector.cpp`)
* Temperatura (`temp_collector.cpp`)
* Uptime (`uptime_collector.cpp`)
* Load Average (`loadavg_collector.cpp`)

### src/config/

Gestiona la lectura, validación y almacenamiento de la configuración del sistema.

Archivos principales:

* `config.hpp`
* `config.cpp`

### src/core/

Núcleo del sistema de monitoreo.

Contiene estructuras fundamentales como:

* `MetricSnapshot`
* `SystemMonitor`
* Tipos compartidos del proyecto

### src/formatters/

Implementa los distintos formatos de salida utilizados por el sistema.

### src/http/

Contiene los handlers HTTP que exponen la información del monitor.

Handlers disponibles:

* Metrics
* History
* Health
* Config
* Alerts
* Prometheus

### src/platform/

Agrupa implementaciones específicas de cada sistema operativo.

Subdirectorios:

* `linux/`
* `windows/`

### src/sampler/

Implementa la recolección periódica de métricas mediante hilos de ejecución.

### src/storage/

Gestiona el almacenamiento y recuperación de snapshots.

Archivos principales:

* `storage.hpp`
* `storage.cpp`
* `schema.hpp`
* `schema.cpp`

### src/utils/

Utilidades compartidas utilizadas por distintos módulos.

#### src/utils/logging/

Implementaciones relacionadas con registro de eventos y mensajes del sistema.

---

## Directorio tests/

Contiene todas las pruebas y artefactos relacionados con aseguramiento de calidad.

### tests/automatizados/

Pruebas ejecutables mediante herramientas automáticas.

#### unit/

Pruebas unitarias de componentes individuales.

Ejemplos:

* Alertas
* Configuración
* Storage

#### integration/

Pruebas de integración entre módulos.

#### e2e/

Pruebas de extremo a extremo del sistema completo.

### tests/casos/

Documentación y definición de casos de prueba.

#### funcionales/

Verifica funcionalidades del sistema.

#### no-funcionales/

Pruebas relacionadas con atributos de calidad.

Subcategorías:

* Rendimiento
* Seguridad

#### regresion/

Casos destinados a validar correcciones y evitar reaparición de errores.

### tests/datos/

Datos utilizados durante la ejecución de pruebas.

#### entrada/

Archivos de entrada.

#### esperados/

Resultados esperados.

### tests/fixtures/

Datos auxiliares y escenarios reutilizables para pruebas.

### tests/core/

Pruebas relacionadas con componentes centrales.

### tests/formatters/

Pruebas de los formateadores de salida.

### tests/manuales/

Material utilizado en pruebas manuales.

#### checklists/

Listas de verificación.

#### evidencias/

Capturas y evidencias de ejecución.

### tests/plan/

Planificación de pruebas.

### tests/reportes/

Reportes generados durante el proceso de validación.

### tests/storage/

Pruebas específicas del módulo de almacenamiento.

---

## Directorio docs/

Contiene la documentación técnica y funcional del proyecto.

Incluye:

* Arquitectura
* API
* Instalación
* Seguridad
* Colectores
* Configuración
* Guías de contribución

### docs/img/

Imágenes utilizadas por la documentación.

---

## Directorio cmake/

Contiene archivos auxiliares utilizados por CMake durante la configuración del proyecto.

### Dependencies.cmake

Define dependencias y configuraciones adicionales de compilación.

---

## Directorio .github/

Contiene configuraciones utilizadas por GitHub.

### ISSUE_TEMPLATE/

Plantillas para la creación de issues.

### workflows/

Pipelines de automatización mediante GitHub Actions.

Workflows identificados:

* `ci.yml`
* `deploy.yml`
* `security.yml`

### PULL_REQUEST_TEMPLATE.md

Plantilla utilizada para la creación de Pull Requests.

### dependabot.yml

Configuración de Dependabot para actualización automática de dependencias.

---

## Otros directorios relevantes

### examples/

Ejemplos de uso del proyecto.

### scripts/

Scripts auxiliares para automatización de tareas.

### security/

Documentación y recursos relacionados con seguridad.

### data/

Archivos de datos utilizados por la aplicación.
