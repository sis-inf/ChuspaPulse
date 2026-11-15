# Dependencias Externas

## Resumen

El proyecto **ChuspaPulse** utiliza un conjunto mínimo de dependencias externas. La principal dependencia es Google Test, utilizada para la ejecución de pruebas automatizadas. No existen dependencias de ejecución (runtime) adicionales fuera de la biblioteca estándar de C++ y las llamadas al sistema proporcionadas por Linux.

## Tabla de dependencias

| Dependencia | Versión | Propósito | Instalación | Obligatoria |
|-------------|----------|------------|-------------|-------------|
| Google Test (gtest) | Repositorios Ubuntu | Pruebas unitarias | apt | Sí |
| clang-format | Repositorios Ubuntu | Formateo de código | apt | No |
| clang-tidy | Repositorios Ubuntu | Análisis estático | apt | No |                |

---

## Instalación de Google Test

### Ubuntu 22.04

Actualizar los repositorios:

```bash
sudo apt update
```

Instalar Google Test:

```bash
sudo apt install -y libgtest-dev
```

Verificar la instalación:

```bash
dpkg -l | grep gtest
```

### Ubuntu 24.04

Actualizar los repositorios:

```bash
sudo apt update
```

Instalar Google Test:

```bash
sudo apt install -y libgtest-dev
```

Verificar la instalación:

```bash
dpkg -l | grep gtest
```

---

## Instalación de clang-format

### Ubuntu 22.04 y 24.04

```bash
sudo apt update
sudo apt install -y clang-format
```

Verificar la instalación:

```bash
clang-format --version
```

---

## Instalación de clang-tidy

### Ubuntu 22.04 y 24.04

```bash
sudo apt update
sudo apt install -y clang-tidy
```

Verificar la instalación:

```bash
clang-tidy --version
```

---

## Dependencias de ejecución (Runtime)

ChuspaPulse **no tiene dependencias de runtime externas**.

La aplicación utiliza únicamente:

* La biblioteca estándar de C++ (Standard Library).
* Las llamadas al sistema (system calls) proporcionadas por Linux.

Por lo tanto, no es necesario instalar bibliotecas adicionales para ejecutar la aplicación una vez compilada.
