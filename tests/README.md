# Guía de pruebas de ChuspaPulse

Este documento describe cómo compilar, ejecutar y extender los tests del proyecto ChuspaPulse.

## Compilar los tests

Para habilitar la compilación de los tests utilizando CMake:

```bash
cmake -B build -DBUILD_TESTS=ON
```

Luego compile el proyecto:

```bash
cmake --build build
```

En sistemas Linux también puede utilizar:

```bash
make
```

si el proyecto ya fue configurado previamente.

## Ejecutar todos los tests

Una vez compilado el proyecto, ejecute todos los tests registrados con CTest:

```bash
ctest --output-on-failure
```

La opción `--output-on-failure` muestra información detallada cuando un test falla.

## Ejecutar un test específico

Para ejecutar únicamente un test cuyo nombre coincida con un patrón:

```bash
ctest -R nombre
```

Ejemplo:

```bash
ctest -R storage
```

## Estructura de carpetas de tests

| Carpeta                         | Descripción                                     |
| ------------------------------- | ----------------------------------------------- |
| tests/automatizados             | Tests automáticos del proyecto                  |
| tests/automatizados/unit        | Tests unitarios                                 |
| tests/automatizados/integration | Tests de integración                            |
| tests/automatizados/e2e         | Tests end-to-end                                |
| tests/core                      | Tests relacionados con tipos y estructuras base |
| tests/storage                   | Tests del módulo de almacenamiento              |
| tests/formatters                | Tests de formateadores de salida                |
| tests/fixtures                  | Archivos de ejemplo utilizados por los tests    |
| tests/datos                     | Datos de entrada y resultados esperados         |
| tests/casos                     | Casos de prueba documentados                    |
| tests/manuales                  | Evidencias y checklists de pruebas manuales     |
| tests/plan                      | Planificación y estrategia de pruebas           |
| tests/reportes                  | Plantillas y reportes de ejecución              |

## Agregar un nuevo test

1. Crear un archivo `.cpp` en la carpeta correspondiente.
2. Implementar los casos de prueba usando Google Test.
3. Registrar el archivo en `tests/CMakeLists.txt`.
4. Reconfigurar el proyecto:

```bash
cmake -B build -DBUILD_TESTS=ON
```

5. Compilar nuevamente:

```bash
cmake --build build
```

6. Ejecutar los tests para verificar que el nuevo caso funciona correctamente.

## Fixtures disponibles

Actualmente existen los siguientes archivos de prueba en `tests/fixtures`:

* `proc_meminfo_sample`
* `proc_net_dev_sample`
* `proc_stat_sample`

Estos archivos simulan información del sistema operativo y permiten ejecutar pruebas reproducibles sin depender del estado real del equipo.

## Archivos de datos

El directorio `tests/datos` contiene ejemplos de configuración y resultados esperados utilizados durante las pruebas automatizadas. Por ejemplo:

* `sample_config.toml`

Se recomienda reutilizar estos archivos antes de crear nuevos datos de prueba.
