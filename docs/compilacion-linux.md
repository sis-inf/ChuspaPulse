# Compilación en Linux

Este documento describe los pasos básicos para instalar las herramientas necesarias y compilar el proyecto en Linux.

## 1. Instalar dependencias

En distribuciones basadas en Debian o Ubuntu, ejecutar:

```bash
sudo apt install g++ cmake
```

`g++` permite compilar código C++, mientras que `cmake` permite configurar el proceso de compilación del proyecto.

## 2. Configurar el proyecto

Desde la raíz del proyecto, ejecutar:

```bash
cmake -B build
```

Este comando crea la carpeta `build` y genera los archivos necesarios para la compilación.

## 3. Nota sobre permisos en `/proc`

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