# Errores comunes y soluciones

## 1. `undefined reference to ICollector::collect()`
- **Causa**: Falta compilar la implementación o no se enlaza el archivo `.cpp` correspondiente.
- **Solución**: Verifica que el archivo esté listado en `CMakeLists.txt` y recompila:
```bash
cmake --build build
```
##2.  fatal error: no such file or directory: 'ChuspaPulse/collector.hpp' 
- **Causa**: Ruta incorrecta o falta incluir la carpeta  include  en los directorios de búsqueda.
- **Solución**: Asegura que en  CMakeLists.txt  esté:
``` cmake
include_directories(${PROJECT_SOURCE_DIR}/include)
```
## 3.  cmake: command not found 
- **Causa**: CMake no está instalado o no está en el  PATH .
- **Solución**:
```bash
sudo apt update && sudo apt install cmake
```
## 4.  make: no rule to make target 
- **Causa**: El directorio de compilación está desactualizado o falta configurar.
- **Solución**: Borra el directorio  build  y vuelve a configurar:
```bash
rm -rf build && cmake -B build
```
## 5.  test: test not found 
- **Causa**: Las pruebas no se compilaron o no se habilitó  BUILD_TESTING .
- **Solución**:
```bash
cmake -B build -DBUILD_TESTING=ON
cmake --build build
```
## 6.  SIGSEGV en tests 
- **Causa**: Acceso a memoria no válida (puntero nulo, fuera de rango).
- **Solución**: Revisa inicialización de objetos y límites de arreglos; usa herramientas como:
```bash
valgrind --leak-check=full ./build/tests/unit/test_...
```
## 7.  clang-format: unknown argument 
- **Causa**: Versión antigua de  clang‑format  o error en el nombre del parámetro.
- **Solución**: Actualiza y verifica opciones:
```bash
sudo apt install clang-format
clang-format --help
```
## 8.  fatal error: 'iostream' file not found 
- **Causa**: Falta el compilador de C++ o está mal configurado.
- **Solución**:
```bash
sudo apt install g++
```
## 9.  undefined symbol: _ZTV8BaseCollector 
- **Causa**: Métodos virtuales puros no implementados o falta definición.
- **Solución**: Implementa todos los métodos declarados en la clase base.
##10.  CMake Error: The source directory does not appear to contain CMakeLists.txt 
- **Causa**: Ejecutas  cmake  desde el lugar equivocado.
- **Solución**: Hazlo desde la raíz del proyecto:
```bash
cmake -B build -S .
```
## 11.  permission denied while trying to open file 
- **Causa**: Permisos insuficientes sobre archivos o rutas.
- **Solución**:
```bash
chmod +x ./build/bin/ChuspaPulse
chown -R usuario:usuario .
```
## 12.  error: expected ';' at end of declaration 
- **Causa**: Falta punto y coma, paréntesis o llave en el código fuente.
- **Solución**: Revisa la línea indicada por el compilador y las líneas anteriores.
