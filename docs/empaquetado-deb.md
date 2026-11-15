# Empaquetado Debian (.deb)

## Introducción

ChuspaPulse permite generar paquetes Debian (`.deb`) mediante CPack.

Este documento explica el proceso para generar un paquete, revisar su contenido antes de distribuirlo e instalarlo en sistemas basados en Debian o Ubuntu.

## Requisitos previos

Antes de generar el paquete `.deb`, se debe configurar y compilar el proyecto.

Desde la raíz del repositorio ejecutar:

```bash
cmake -S . -B build
cmake --build build
```

Estos comandos generan el directorio de compilación y construyen el proyecto utilizando la configuración definida por CMake.

## Generación del paquete `.deb`

Ingresar al directorio de compilación:

```bash
cd build
```

Generar el paquete Debian utilizando CPack:

```bash
cpack -G DEB
```

Después de ejecutar el comando, CPack generará un archivo con extensión `.deb`.

El nombre exacto del archivo generado puede variar dependiendo de la versión y configuración del proyecto.

Ejemplo:

```text
ChuspaPulse-0.1.0-Linux.deb
```

## Verificación del paquete generado

Antes de distribuir el paquete, se recomienda verificar la información y el contenido incluido.

### Revisar contenido del paquete

Para mostrar los archivos incluidos dentro del paquete y sus rutas de instalación:

```bash
dpkg -c paquete.deb
```

Ejemplo:

```bash
dpkg -c ChuspaPulse-0.1.0-Linux.deb
```

Este comando permite comprobar qué archivos serán instalados por el paquete.

### Revisar información del paquete

Para consultar la información del paquete generado:

```bash
dpkg -I paquete.deb
```

Ejemplo:

```bash
dpkg -I ChuspaPulse-0.1.0-Linux.deb
```

Este comando muestra información del paquete como nombre, versión, arquitectura y otros datos de configuración.

## Instalación del paquete

Para instalar el paquete generado en un sistema Debian o Ubuntu:

```bash
sudo dpkg -i paquete.deb
```

Ejemplo:

```bash
sudo dpkg -i ChuspaPulse-0.1.0-Linux.deb
```

Después de la instalación, el usuario puede verificar la disponibilidad del programa según la configuración incluida en el paquete.

## Notas

- Se recomienda verificar el contenido del paquete antes de distribuirlo.
- Las rutas exactas instaladas pueden consultarse mediante `dpkg -c`.
- La estructura final del paquete depende de la configuración CPack utilizada por el proyecto.
