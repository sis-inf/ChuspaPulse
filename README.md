# Pulso

[![CI](https://github.com/sis-inf/pulso/actions/workflows/ci.yml/badge.svg)](https://github.com/sis-inf/pulso/actions/workflows/ci.yml)
[![Licencia MIT](https://img.shields.io/badge/licencia-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
Plataforma en C++ para gestionar y monitorear información clave de forma eficiente.

## Inicio Rápido

```bash
sudo apt install git cmake g++
git clone https://github.com/sis-inf/pulso.git && cd pulso
cmake -S . -B build && cmake --build build
./build/pulso --once
```

Salida esperada:

```text
Estado del sistema: OK
```

## ¿Qué es?

Pulso es una aplicación desarrollada en C++ que permite gestionar y visualizar información relevante de manera centralizada. Está pensada para ofrecer alto rendimiento y control directo sobre los recursos del sistema.

## ¿Para quién es?

Pulso está dirigido a:

- Desarrolladores que trabajan con sistemas de alto rendimiento
- Equipos que requieren herramientas eficientes y ligeras
- Proyectos que necesitan control preciso sobre memoria y procesamiento

## ¿Qué problema resuelve?

Pulso ayuda a resolver:

- La dispersión de información en múltiples fuentes
- La falta de herramientas rápidas y eficientes
- El alto consumo de recursos en soluciones tradicionales

Gracias a su implementación en C++, Pulso permite una ejecución más rápida y optimizada.

## Instalación

**Requisitos del sistema**
- Sistema operativo soportado actualmente: **Linux (Kernel 5.4+)** y **Windows 10/11**. **macOS 12+ aún no está soportado.**
- Compilador de C++ (GCC, Clang o MSVC)
- CMake (recomendado)
- Git

### Soporte de Plataformas

| Sistema Operativo | Estado de Soporte | Colectores de Métricas |
| :--- | :---: | :---: |
| **Linux (Kernel 5.4+)** | Soporte Pleno | 100% Funcional |
| **Windows 10/11** | Soporte Pleno | 100% Funcional |
| **macOS 12+** | No soportado actualmente | Colectores nativos pendientes |

### Pasos básicos

```bash
# Clonar el repositorio
git clone https://github.com/tu-usuario/pulso.git

# Entrar al proyecto
cd pulso
```

---

> [!NOTE]
> Estos pasos corresponden a la versión final esperada del proyecto.

```bash
# Crear carpeta de build
mkdir build && cd build

# Generar archivos de compilación
cmake ..

# Compilar
cmake --build .
```

Uso rápido:

```bash
# Ejecutar el programa (puede variar según el sistema)
./pulso
```

En Windows:

```bash
pulso.exe
```
## Compilación rápida

```bash
cmake -S . -B build
make -C build
./build/pulso
```

Para una guía completa de instalación consulte:

[docs/instalacion.md](docs/instalacion.md)

---

## Compilación con AddressSanitizer

AddressSanitizer (ASAN) es una herramienta para detectar errores de memoria como accesos inválidos, leaks de memoria, y otros problemas. Es más rápido que Valgrind y ideal para desarrollo.

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

## Ejemplo de salida

Ejecutar:

```bash
./build/pulso --once --format json
```

Salida:

```json
{
  "status": "OK",
  "cpu": {
    "usage_percent": 23
  },
  "memory": {
    "usage_percent": 40
  },
  "disk": {
    "usage_percent": 47
  },
  "network": {
    "status": "connected"
  }
}
```

---

---
## Ejemplo basico de uso esperado

```bash
# Entrada
./pulso

# Salida
¡Bienvenido a Pulso!
Seleccione una opción:
1. Ver estado
2. Actualizar datos
3. Salir

# Entrada
> 1

# Salida
Estado del sistema: OK
```
---
> [!IMPORTANT]
> Este proyecto se encuentra en desarrollo activo. Los pasos de instalación y ejecución pueden cambiar en futuras versiones.
---
## Compilación con Makefile (sin CMake)
 
Para entornos ligeros donde CMake no está disponible, el proyecto incluye un
`Makefile` alternativo listo para usar.
 
### Requisitos
 
| Herramienta | Versión mínima |
|-------------|---------------|
| `g++` / `clang++` | C++17 |
| GNU Make | 4.x |
 
### Uso rápido
 
```bash
# Compilar el proyecto
make
 
# Usar un compilador distinto (ej. clang++)
make CXX=clang++
 
# Ejecutar los tests
make test
 
# Eliminar artefactos de compilación
make clean
```
 
### Variables configurables
 
| Variable | Valor por defecto | Descripción |
|----------|------------------|-------------|
| `CXX` | `g++` | Compilador C++ |
| `CXXFLAGS` | `-std=c++17 -Wall` | Flags de compilación |
 
> **Nota:** el operador `?=` en `CXX` permite sobreescribir el compilador
> desde la línea de comandos o desde la variable de entorno del sistema sin
> modificar el Makefile.
 
### Estructura esperada
 
```
project/
├── src/          # Fuentes principales (*.cpp)
├── tests/        # Fuentes de pruebas  (*.cpp)
├── build/        # Artefactos generados (ignorado por git)
└── Makefile
```
 
---

## ✨ Características

### 🌐 Soporte Multiplataforma
- Compatibilidad actual con **Linux** y **Windows**.
- El soporte nativo para **macOS** permanece pendiente.

### 📊 Métricas monitoreadas
- CPU
- RAM
- Disco
- Red
- Procesos activos
- Carga del sistema
- Uptime

### 📤 Formatos de salida
- JSON
- Prometheus
- CSV
- Texto

### 🌐 Endpoints HTTP
- Consulta en tiempo real
- Exportación de datos

### ⚠️ Sistema de alertas
- Configuración por umbrales
- Notificación de valores fuera de rango

### ⚙️ Modo de uso
- Modo `--once`: ejecución única para scripting

### Recompilación incremental
 
El Makefile compila únicamente los archivos `.cpp` que hayan cambiado desde
la última build, gracias al seguimiento de dependencias de Make sobre los
archivos objeto (`.o`) en `build/`.
---

## Documentación

Ver la carpeta [docs/](docs/)

Guía completa de instalación:

## Documentación

[Índice General de la Documentación](docs/indice-general.md)

[docs/instalacion.md](docs/instalacion.md)

## Contribuir
Ver [CONTRIBUTING.md](CONTRIBUTING.md)

## Licencia
MIT — ver [LICENSE](LICENSE)