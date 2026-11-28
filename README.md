# ChuspaPulse

[![CI](https://github.com/sis-inf/ChuspaPulse/actions/workflows/ci.yml/badge.svg)](https://github.com/sis-inf/ChuspaPulse/actions/workflows/ci.yml)
[![Licencia MIT](https://img.shields.io/badge/licencia-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

Agente en C++ para monitorear métricas del sistema (CPU, RAM, disco, red, procesos, carga y uptime) y exportarlas en JSON, CSV, Prometheus o texto.

## Inicio Rápido

```bash
sudo apt install git cmake g++
git clone https://github.com/sis-inf/ChuspaPulse.git && cd ChuspaPulse
cmake -S . -B build && cmake --build build
./build/bin/chuspapulse --once --format json
```

`--once` hace una sola lectura, imprime el resultado y termina. La salida es un JSON con el `timestamp` del snapshot y una lista `metricas`; cada métrica tiene `nombre`, `valor`, `unidad` y `timestamp`. El comando imprime todo en una sola línea; aquí se indenta para leerlo mejor y los valores son de ejemplo:

```json
{
  "timestamp": 1795800000,
  "metricas": [
    {"nombre": "cpu.usage", "valor": 12.5, "unidad": "porcentaje", "timestamp": 1795800000},
    {"nombre": "cpu.cores", "valor": 4, "unidad": "cantidad", "timestamp": 1795800000},
    {"nombre": "memory.used", "valor": 3221225472, "unidad": "bytes", "timestamp": 1795800000},
    {"nombre": "disk.total", "valor": 256000000000, "unidad": "bytes", "timestamp": 1795800000},
    {"nombre": "disk.used", "valor": 120000000000, "unidad": "bytes", "timestamp": 1795800000},
    {"nombre": "disk.free", "valor": 136000000000, "unidad": "bytes", "timestamp": 1795800000},
    {"nombre": "net.rx", "valor": 1048576, "unidad": "bytes", "timestamp": 1795800000},
    {"nombre": "net.tx", "valor": 524288, "unidad": "bytes", "timestamp": 1795800000},
    {"nombre": "procesos.running", "valor": 2, "unidad": "cantidad", "timestamp": 1795800000},
    {"nombre": "procesos.blocked", "valor": 0, "unidad": "cantidad", "timestamp": 1795800000},
    {"nombre": "procesos.total", "valor": 215, "unidad": "cantidad", "timestamp": 1795800000},
    {"nombre": "loadavg.1", "valor": 0.42, "unidad": "carga", "timestamp": 1795800000},
    {"nombre": "loadavg.5", "valor": 0.35, "unidad": "carga", "timestamp": 1795800000},
    {"nombre": "loadavg.15", "valor": 0.3, "unidad": "carga", "timestamp": 1795800000},
    {"nombre": "uptime.seconds", "valor": 86400, "unidad": "segundos", "timestamp": 1795800000}
  ]
}
```

Si el equipo tiene batería o sensor de temperatura, también aparecen `bateria.porcentaje`, `bateria.cargando` y `temperatura.cpu`.

Otros formatos: `--format csv`, `--format prometheus` y `--format text`. Para ver todas las opciones: `./build/bin/chuspapulse --help`.

## ¿Qué es?

ChuspaPulse es un agente de monitoreo escrito en C++17. Lee las métricas del sistema operativo y las entrega con bajo consumo de recursos.

## ¿Para quién es?

ChuspaPulse está dirigido a:

- Desarrolladores que trabajan con sistemas de alto rendimiento
- Equipos que requieren herramientas eficientes y ligeras
- Proyectos que necesitan control preciso sobre memoria y procesamiento

## ¿Qué problema resuelve?

ChuspaPulse ayuda a resolver:

- La dispersión de información en múltiples fuentes
- La falta de herramientas rápidas y eficientes
- El alto consumo de recursos en soluciones tradicionales

Gracias a su implementación en C++, ChuspaPulse permite una ejecución más rápida y optimizada.

## Modos de uso

- **`--once`:** una sola lectura, impresa en la salida estándar. Pensado para scripts.
- **Modo daemon (sin `--once`):** muestrea cada cierto intervalo, guarda los snapshots en SQLite y expone una API HTTP (por defecto en el puerto `8080`). No tiene menú interactivo ni imprime métricas en pantalla. La configuración está en `pulso.toml`.

## Soporte de plataformas

| Sistema operativo | Estado | Detalle |
| :--- | :---: | :--- |
| **Linux (Kernel 5.4+)** | Soportado | Los colectores leen de `/proc`, `/sys` y `statvfs`. |
| **Windows 10/11** | Parcial, en desarrollo | Solo CPU y uptime tienen colector específico; el resto no tiene implementación. Ver [colectores](docs/colectores.md) y [requerimientos](docs/requerimientos.md). |
| **macOS 12+** | No soportado | Colectores nativos pendientes (ver [límites conocidos](docs/limites-conocidos.md)). |

## Instalación y compilación

Requisitos: Linux, un compilador C++17 (GCC o Clang), CMake 3.16 o superior y Git. La primera compilación descarga las dependencias, así que necesita conexión a internet.

```bash
sudo apt install git cmake g++
git clone https://github.com/sis-inf/ChuspaPulse.git && cd ChuspaPulse
cmake -S . -B build
cmake --build build
./build/bin/chuspapulse --once --format json
```

También puedes usar el `Makefile`, que invoca los mismos comandos de CMake y compila además las pruebas:

```bash
make          # Compila el proyecto
make test     # Ejecuta las pruebas
make clean    # Elimina el directorio build
```

Guía completa de instalación: [docs/instalacion.md](docs/instalacion.md)

> [!IMPORTANT]
> Este proyecto se encuentra en desarrollo activo. Los pasos de instalación y ejecución pueden cambiar en futuras versiones.

## Compilación con AddressSanitizer

AddressSanitizer (ASAN) detecta errores de memoria como accesos inválidos y fugas. Es más rápido que Valgrind y es útil durante el desarrollo.

### Compilar tests con ASAN

```bash
cmake -S . -B build -DBUILD_TESTS=ON -DBUILD_WITH_ASAN=ON
cmake --build build
```

### Ejecutar tests con ASAN

```bash
cd build
ctest
```

O ejecutar un test específico:

```bash
./build/bin/test_types
./build/bin/test_config
./build/bin/test_storage
```

> [!NOTE]
> AddressSanitizer solo se aplica a los tests. El build de producción no incluye los flags de ASAN para mantener el rendimiento.

---

## ✨ Características

### 🌐 Plataformas
- **Linux** soportado.
- **Windows** parcial (solo CPU y uptime).
- El soporte nativo para **macOS** permanece pendiente.

### 📊 Métricas monitoreadas
- CPU
- RAM
- Disco
- Red
- Procesos activos
- Carga del sistema
- Uptime
- Batería y temperatura (si el equipo las tiene)

### 📤 Formatos de salida
- JSON
- Prometheus
- CSV
- Texto

### 🌐 Endpoints HTTP (modo daemon)
- Consulta en tiempo real
- Exportación de datos

### ⚠️ Sistema de alertas
- Configuración por umbrales
- Notificación de valores fuera de rango

### ⚙️ Modos de uso
- `--once`: ejecución única para scripting
- Modo daemon: muestreo continuo y API HTTP

---

## Documentación

- [Índice general de la documentación](docs/indice-general.md)
- [Guía de instalación](docs/instalacion.md)
- Carpeta [docs/](docs/)

## Contribuir
Ver [CONTRIBUTING.md](CONTRIBUTING.md)

## Licencia
MIT — ver [LICENSE](LICENSE)
