# Compilación en Linux

Este documento describe los pasos básicos para instalar las herramientas necesarias y compilar el proyecto en Linux.

## 1. Instalar dependencias

El proyecto requiere un compilador de C++17, CMake 3.16 o superior, Git y los archivos de desarrollo de SQLite3.

Git es necesario porque CMake descarga varias dependencias con FetchContent durante la configuración, por lo que la primera compilación requiere conexión a internet.

### Distribuciones basadas en Debian (Debian, Ubuntu, Linux Mint)

```bash
sudo apt update
sudo apt install build-essential cmake git libsqlite3-dev
```

### Distribuciones basadas en RHEL (Fedora, CentOS Stream, Rocky Linux, AlmaLinux)

```bash
sudo dnf install gcc-c++ cmake git sqlite-devel
```

En versiones antiguas de CentOS o RHEL se puede usar `yum` en lugar de `dnf`.

Las siguientes dependencias no se instalan manualmente: CMake las descarga automáticamente al configurar el proyecto.

| Dependencia | Uso |
|---|---|
| cpp-httplib | servidor HTTP de métricas |
| nlohmann/json | serialización JSON |
| toml++ | lectura del archivo de configuración |
| SQLiteCpp | almacenamiento de métricas |
| GoogleTest | pruebas automatizadas |

## 2. Configurar el proyecto

Desde la raíz del proyecto, ejecutar:

```bash
cmake -B build
```

Este comando crea la carpeta `build` y genera los archivos necesarios para la compilación.

Para compilar también la suite de pruebas, agregar la opción `BUILD_TESTS`:

```bash
cmake -B build -DBUILD_TESTS=ON
```

Esta opción está desactivada por defecto. Al activarla, CMake descarga GoogleTest y agrega los ejecutables de prueba.

## 3. Compilar

```bash
cmake --build build
```

El binario resultante queda en `build/bin/`.

## 4. Ejecutar las pruebas con ctest

Las pruebas se ejecutan con `ctest` una vez compilado el proyecto con `-DBUILD_TESTS=ON`:

```bash
ctest --test-dir build --output-on-failure
```

La opción `--output-on-failure` muestra la salida completa de las pruebas que fallan.

Si la versión instalada de CMake es anterior a la 3.20 y no reconoce `--test-dir`, ejecutar desde la carpeta de compilación:

```bash
cd build
ctest --output-on-failure
```

Otras opciones útiles:

```bash
# Listar las pruebas sin ejecutarlas
ctest --test-dir build -N

# Ejecutar una prueba específica por nombre
ctest --test-dir build -R test_formatter_json

# Ejecutar en paralelo con 4 procesos
ctest --test-dir build -j4
```

## 5. Nota sobre permisos en `/proc`

En Linux, algunos programas pueden acceder a información del sistema mediante `/proc`.

Si aparecen errores relacionados con permisos en `/proc`, se debe verificar que el programa tenga los permisos necesarios. No se recomienda modificar permisos del sistema sin conocer su impacto, ya que `/proc` contiene información sensible del sistema operativo y de los procesos en ejecución.

### Versiones de Ubuntu soportadas

#### Ubuntu 22.04 LTS
```bash
sudo apt update
sudo apt install build-essential cmake git
# GCC 11 por defecto
```
## Ubuntu 24.04 LTS
```bash
sudo apt update
sudo apt install build-essential cmake git
# GCC 13 por defecto
```
## Compilación alternativa con Clang
Primero instala el compilador:
```bash
sudo apt install clang
```
## Con CMake
```bash
# Configurar usando Clang
CC=clang CXX=clang++ cmake -B build

# Compilar con banderas de advertencia
cmake --build build -- -Wall -Wextra -Wpedantic
```
## Directamente con Clang++
```bash
clang++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp -o pulso
```
---

## Build estático (binario standalone)

Por defecto, el binario de ChuspaPulse enlaza dinámicamente con las librerías del sistema (`libgcc`, `libstdc++`, etc.). Para producir un binario standalone que no dependa de estas librerías en tiempo de ejecución, usa la opción `PULSO_STATIC_LINK`:

```bash
cmake -B build -DPULSO_STATIC_LINK=ON
cmake --build build
```

Puedes verificar las dependencias dinámicas del binario con:

```bash
ldd build/bin/pulso
```

Con `PULSO_STATIC_LINK=ON`, la salida mostrará significativamente menos dependencias dinámicas. Con `PULSO_STATIC_LINK=OFF` (por defecto), el comportamiento es idéntico al build estándar.

### Limitaciones conocidas del enlace estático

- **glibc no se enlaza estáticamente de forma confiable en Linux.** El enlace estático completo con glibc (`-static`) puede causar problemas en tiempo de ejecución relacionados con resolución de nombres DNS y NSS. Por eso `PULSO_STATIC_LINK` solo enlaza estáticamente `libgcc` y `libstdc++`, no glibc.
- **SQLite y las dependencias de FetchContent** se enlazan estáticamente por defecto al compilarse como librerías estáticas (`.a`), independientemente de esta opción.
- Esta opción es útil para distribución en contenedores Docker o sistemas donde no se puede garantizar la versión de `libstdc++` instalada.