# Guía de instalación de Pulso

Este documento describe los pasos necesarios para instalar y ejecutar Pulso en un entorno local.

---

## Requisitos previos

Antes de instalar Pulso, asegúrese de contar con lo siguiente:

| Requisito | Versión mínima | Notas |
|---|---|---|
| Sistema operativo | — | Linux o Windows; macOS aún no está soportado |
| Compilador C++ | C++17 | GCC, Clang o MSVC |
| CMake | 3.16 | |
| Git | — | Para clonar el repositorio |

### Instalar dependencias
 
**Linux (Debian/Ubuntu)**
```bash
sudo apt install g++ cmake git
```
 
**macOS**

> macOS aún no cuenta con collectors nativos completos, por lo que no se considera una plataforma soportada actualmente.

**Windows**
 
- Instale [CMake](https://cmake.org/download/) y marque la opción para agregarlo al PATH durante la instalación.
- Instale [Git](https://git-scm.com/download/win).
- Instale [Visual Studio](https://visualstudio.microsoft.com/) con la carga de trabajo **"Desarrollo de escritorio con C++"**, que incluye el compilador MSVC.

---

## Clonar el repositorio

Ejecute el siguiente comando para clonar el proyecto:

```bash
git clone https://github.com/sis-inf/pulso.git
cd pulso
```

---

## Compilar el proyecto
 
### Linux
 
```bash
# Configurar el proyecto y crear la carpeta build
cmake -B build
 
# Compilar
cmake --build build
```
 
### Windows
 
Abra el **Developer Command Prompt** de Visual Studio y ejecute:
 
```bash
# Configurar el proyecto y crear la carpeta build
cmake -B build
 
# Compilar
cmake --build build --config Release
```
 
> El binario resultante se encuentra en `build/bin/`.
 
---
 
## Ejecutar el programa
 
### Linux
 
```bash
./build/bin/pulso
```
 
### Windows
 
```bash
build\bin\Release\pulso.exe
```
 
---
 
## Notas adicionales
 
- En Linux, si aparecen errores de permisos al acceder a `/proc`, verifique que el usuario tenga los permisos necesarios. No se recomienda modificar permisos del sistema sin conocer su impacto.
- Para compilar sin ejecutar los tests, agregue `-DPULSO_BUILD_TESTS=OFF` al comando de configuración:

```bash
cmake -B build -DPULSO_BUILD_TESTS=OFF
```

---

## Instalación de dependencias

### Ubuntu 22.04 LTS
```bash
sudo apt update
sudo apt install build-essential cmake git
# g++ versión ≥ 11 ya está disponible por defecto
```
## Ubuntu 24.04 LTS
```bash
sudo apt update
sudo apt install build-essential cmake git
# Incluye versiones más nuevas de compiladores y herramientas
```
## Verificación de versiones
 
Antes de compilar, confirma que cumples los requisitos:
```bash
g++ --version   # debe mostrar ≥ 11
cmake --version # debe mostrar ≥ 3.16
```
## Problemas comunes y soluciones
 
1. «g++: error: unrecognized command‑line option ‘‑std=c++20’»
- Causa: Versión de compilador antigua.
- Solución: Actualizar paquetes o usar distribución soportada:
```bash
sudo apt upgrade g++
```
2. «CMake Error: CMAKE_CXX_COMPILER not set, after EnableLanguage»
- Causa: Falta instalar el paquete completo de compiladores.
- Solución:
```bash
sudo apt install --reinstall build-essential
```
3. «fatal error: ‘filesystem’ file not found»
- Causa: La librería  std::filesystem  requiere C++17 o superior y compilador moderno.
- Solución: Asegurar bandera  -std=c++17  o  -std=c++20  y verificar versión ≥ 11.
```bash
g++ --version
```
