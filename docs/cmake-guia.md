# Guía de CMake del proyecto ChuspaPulse

## Introducción

El proyecto **ChuspaPulse** utiliza **CMake** como sistema de construcción para organizar la compilación del código fuente, administrar las dependencias externas y generar los ejecutables del proyecto. La configuración del proceso de compilación se encuentra distribuida en diferentes archivos `CMakeLists.txt`, cada uno con una función específica dentro de la estructura del proyecto.

Esta guía tiene como objetivo explicar cómo está organizado el sistema de build de ChuspaPulse, describiendo la función de cada archivo de configuración, los comandos necesarios para compilar el proyecto y el procedimiento para agregar nuevos archivos fuente. Está dirigida principalmente a estudiantes y colaboradores que comienzan a trabajar con CMake y desean comprender la estructura utilizada en este repositorio.

---

# ¿Qué es CMake y por qué se utiliza?

CMake es una herramienta que permite definir de forma organizada el proceso de compilación de un proyecto. En lugar de crear manualmente archivos de construcción para cada sistema operativo o compilador, CMake utiliza archivos llamados `CMakeLists.txt`, donde se especifican las reglas necesarias para construir el proyecto.

En ChuspaPulse, CMake permite:

* Mantener una estructura organizada del proyecto.
* Configurar el estándar de C++ utilizado.
* Administrar dependencias externas.
* Generar el ejecutable principal.
* Compilar las pruebas cuando están habilitadas.
* Facilitar el crecimiento del proyecto mediante una estructura modular.

Gracias a esta organización, resulta más sencillo incorporar nuevos módulos y mantener el proyecto a medida que aumenta su tamaño.

---

# Estructura del sistema de build

El sistema de compilación de ChuspaPulse está dividido en varios archivos `CMakeLists.txt`, cada uno con una responsabilidad específica.

## CMakeLists.txt principal

El archivo ubicado en la raíz del proyecto es el punto de entrada del sistema de compilación.

Entre sus principales funciones se encuentran:

* Definir la versión mínima de CMake requerida.
* Registrar el proyecto con el nombre **ChuspaPulse**.
* Configurar el estándar **C++17**.
* Definir las opciones de compilación.
* Configurar los directorios donde se generan los ejecutables y bibliotecas.
* Cargar las dependencias externas mediante `cmake/Dependencies.cmake`.
* Incorporar el directorio `src`, donde se encuentra el código fuente principal.
* Incorporar el directorio `tests` únicamente cuando las pruebas están habilitadas.

Esta organización permite mantener separadas las diferentes responsabilidades del proceso de compilación.

---

## src/CMakeLists.txt

Este archivo controla la compilación del ejecutable principal del proyecto.

Primero crea el ejecutable mediante:

```cmake
add_executable(chuspapulse main.cpp)
```

Posteriormente incorpora todos los archivos fuente utilizando `target_sources()`.

En este archivo también se configuran:

* Los directorios de inclusión mediante `target_include_directories()`.
* Las bibliotecas utilizadas por el proyecto mediante `target_link_libraries()`.
* El estándar C++17 mediante `target_compile_features()`.
* Las opciones del compilador mediante `target_compile_options()`.

Toda esta configuración permite que el ejecutable principal se genere correctamente utilizando los diferentes módulos del proyecto.

---

## tests/CMakeLists.txt

Este archivo administra la compilación de las pruebas unitarias.

Primero habilita el sistema de pruebas mediante:

```cmake
enable_testing()
```

Después crea diferentes ejecutables para los casos de prueba existentes, como por ejemplo:

* test_types
* test_config
* test_storage
* test_disk_usage
* test_formatter_prometheus

Cada prueba incluye los encabezados necesarios, enlaza las bibliotecas correspondientes y finalmente se registra utilizando `gtest_discover_tests()` para que pueda ejecutarse mediante CTest.

---

## cmake/Dependencies.cmake

Este archivo centraliza la administración de las dependencias externas utilizadas por ChuspaPulse.

Las bibliotecas se descargan automáticamente mediante `FetchContent`, evitando que el desarrollador tenga que agregarlas manualmente.

Entre las principales dependencias utilizadas se encuentran:

* cpp-httplib
* nlohmann/json
* toml++
* SQLiteCpp

Cuando las pruebas están habilitadas también se descarga GoogleTest, permitiendo compilar los casos de prueba definidos dentro del directorio `tests`.

Esta separación facilita el mantenimiento del proyecto y evita duplicar configuraciones en otros archivos.

---

# Compilación paso a paso

Para generar la carpeta de compilación se utiliza el siguiente comando:

```bash
cmake -S . -B build
```

Este comando crea el directorio `build` y genera los archivos necesarios para la compilación.

Una vez configurado el proyecto, la compilación se realiza mediante:

```bash
cmake --build build
```

Estos son los comandos recomendados para compilar el proyecto desde la raíz del repositorio.

---

# Opciones de compilación

El proyecto define diferentes opciones que permiten modificar el comportamiento del proceso de construcción.

## BUILD_TESTS

Cuando esta opción está habilitada, el sistema incorpora el directorio `tests` y compila las pruebas unitarias del proyecto.

Si la opción permanece deshabilitada, únicamente se genera el ejecutable principal.

## CHUSPAPULSE_BUILD_TESTS

Esta opción mantiene compatibilidad con configuraciones anteriores del proyecto y se sincroniza con `BUILD_TESTS`. Cuando cualquiera de las dos opciones se activa, ambas quedan habilitadas para permitir la compilación de las pruebas.

## BUILD_WITH_ASAN

La opción `BUILD_WITH_ASAN` suele utilizarse en proyectos C++ para habilitar **AddressSanitizer (ASan)**, una herramienta que ayuda a detectar errores de memoria durante la ejecución, como accesos fuera de límites, uso de memoria liberada o fugas de memoria.

En la versión actual del proyecto ChuspaPulse esta opción no se encuentra configurada en los archivos `CMakeLists.txt` revisados. Si se implementa en el futuro, permitirá realizar compilaciones orientadas a la depuración y detección de errores de memoria.

---

# Cómo agregar un nuevo archivo fuente

Para incorporar un nuevo archivo fuente al proyecto se recomienda seguir el siguiente procedimiento:

1. Crear el nuevo archivo `.cpp` dentro del directorio correspondiente.
2. Agregar el archivo en la lista definida por `target_sources()` dentro de `src/CMakeLists.txt`.
3. Guardar los cambios realizados.
4. Ejecutar nuevamente la configuración del proyecto:

```bash
cmake -S . -B build
```

5. Compilar el proyecto:

```bash
cmake --build build
```

Después de estos pasos, el nuevo archivo formará parte del ejecutable principal del proyecto.

---

# Buenas prácticas

Durante el mantenimiento del sistema de build se recomienda:

* Mantener organizados los archivos `CMakeLists.txt`.
* Agregar únicamente los archivos fuente necesarios.
* Evitar modificar configuraciones globales sin necesidad.
* Verificar que el proyecto compile correctamente después de cada cambio.
* Mantener separada la configuración del código fuente, las dependencias y las pruebas.

Seguir estas prácticas facilita el mantenimiento del proyecto y reduce la posibilidad de errores durante la compilación.

---

# Conclusión

El sistema de construcción de ChuspaPulse utiliza una estructura modular basada en CMake que facilita la organización del código, la administración de dependencias y la compilación tanto del ejecutable principal como de las pruebas. Comprender la función de cada archivo `CMakeLists.txt` permite realizar modificaciones de forma ordenada, incorporar nuevos módulos con mayor facilidad y mantener un proceso de compilación consistente para todos los colaboradores del proyecto.
