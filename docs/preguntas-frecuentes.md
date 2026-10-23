# Preguntas frecuentes

Respuestas a las dudas más comunes sobre ChuspaPulse.

---

## ¿Qué es ChuspaPulse?

ChuspaPulse es una herramienta de línea de comandos desarrollada en C++ que permite recolectar y visualizar métricas del sistema en tiempo real. Monitorea recursos como CPU, memoria RAM, disco y red, y está diseñada para ofrecer alto rendimiento con bajo consumo de recursos.

---

## ¿En qué plataformas funciona ChuspaPulse?

ChuspaPulse es compatible con los siguientes sistemas operativos:

- **Linux** — Kernel 5.4 o superior
- **Windows** — Windows 10 u 11
- **macOS** — macOS 12 o superior

---

## ¿Qué métricas puede monitorear ChuspaPulse?

ChuspaPulse monitorea las siguientes métricas del sistema:

| Métrica | Unidad | Frecuencia |
|---|---|---|
| CPU | Porcentaje (%) | Cada 1 segundo |
| Memoria RAM | MB / % | Cada 1 segundo |
| Disco | GB / % | Cada 5 segundos |
| Red (entrada/salida) | Bytes/segundo | Cada 1 segundo |
| Procesos activos | Cantidad | Cada 2 segundos |
| Carga del sistema | Load average | Cada 5 segundos |

Las frecuencias de muestreo pueden ajustarse según el entorno de ejecución.

---

## ¿Cómo se instala ChuspaPulse?

Para instalar ChuspaPulse se necesita un compilador C++ (GCC, Clang o MSVC), CMake 3.16 o superior y Git. Los pasos básicos son:

```bash
# Clonar el repositorio
git clone https://github.com/sis-inf/ChuspaPulse.git
cd ChuspaPulse

# Compilar
cmake -B build
cmake --build build
```

Para instrucciones detalladas por plataforma, consulte [instalacion.md](instalacion.md).

---

## ¿Cómo se ejecuta ChuspaPulse?

Una vez compilado, el binario se encuentra en `build/bin/`. Para ejecutarlo:

**Linux y macOS:**

```bash
./build/bin/ChuspaPulse
```

**Windows:**

```text
build\bin\Release\ChuspaPulse.exe
```

---

## ¿Cómo puedo contribuir al proyecto?

El proyecto usa el **Forking Workflow**. Los pasos básicos son:

1. Haz un fork del repositorio en tu cuenta de GitHub.
2. Clona tu fork y agrega el repositorio original como `upstream`.
3. Sincroniza con `upstream/dev` antes de empezar a trabajar.
4. Crea una rama siguiendo la convención `tipo/descripcion-corta`.
5. Realiza tus cambios con commits descriptivos siguiendo la convención del proyecto.
6. Abre un Pull Request hacia `dev` referenciando el issue con `Closes #N`.

Para más detalles, consulte [CONTRIBUTING.md](../CONTRIBUTING.md).

---

## ¿Qué compilador necesito para usar ChuspaPulse?

ChuspaPulse requiere un compilador compatible con el estándar **C++17**. Las opciones soportadas según el sistema operativo son:

- **Linux** — GCC 11 o superior
- **macOS** — Clang 13 o superior (incluido con las herramientas de Xcode)
- **Windows** — MSVC 2019 o superior (incluido con Visual Studio)

---

## Preguntas frecuentes sobre C++ y CMake

### ¿Por qué usar CMake y no un Makefile directo?

CMake funciona en Windows, Linux y macOS, genera archivos de compilación automáticamente y evita escribir reglas complejas manualmente.

```cmake
cmake_minimum_required(VERSION 3.16)
project(ChuspaPulse LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(ChuspaPulse main.cpp)
```

¿Cómo agrego un nuevo archivo .cpp al proyecto?

Edita el archivo CMakeLists.txt:

```cmake
add_executable(ChuspaPulse
    main.cpp
    monitoreo.cpp
    nuevo_archivo.cpp
)
```

Comando para aplicar:

```bash
cmake -B build && cmake --build build
```

¿Qué es la carpeta build/ y puedo borrarla?

Guarda archivos temporales. Sí se puede borrar sin riesgo:

```bash
rm -rf build          # Linux / macOS
rd /s /q build        # Windows
```

Para recrearla:

```bash
cmake -B build
```

¿Por qué falla clang-format con mi archivo?

Se arregla automáticamente con:

```bash
clang-format -i archivo.cpp
```

¿Cómo ejecuto solo una prueba de Google Test?

```bash
cd build && ctest -R "NombreDeLaPrueba" -V
# O directamente:
./build/tests/ChuspaPulse-tests --gtest_filter=NombreClase.NombrePrueba
```

¿Qué es una fuga de memoria y cómo revisarla?

Ocurre si reservas memoria y no la liberas:

```bash
valgrind --leak-check=full ./build/bin/ChuspaPulse
```

¿Cómo limpiar y recompilar todo desde cero?

```bash
rm -rf build && mkdir build && cd build && cmake .. && cmake --build .
```

¿Cómo agregar advertencias al compilador?

En CMakeLists.txt:

```cmake
target_compile_options(ChuspaPulse PRIVATE -Wall -Wextra -pedantic -O2)
```
