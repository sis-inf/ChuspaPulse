# Política de versionado semántico de ChuspaPulse

## Introducción

ChuspaPulse utiliza **Versionado Semántico (Semantic Versioning o SemVer)** para identificar las versiones del proyecto de forma consistente y predecible.

Actualmente el proyecto define su versión en `CMakeLists.txt` mediante:

```cmake
project(ChuspaPulse VERSION 0.1.0)
```

El formato de una versión es:

```
MAJOR.MINOR.PATCH
```

Por ejemplo:

```
1.4.2
```

---

# Significado de cada componente

## MAJOR

Se incrementa cuando existen cambios incompatibles con versiones anteriores.

Ejemplos:

- Cambios en el esquema de almacenamiento (Storage) que no sean migrables.
- Eliminación o modificación incompatible de la API.
- Cambios que obliguen a modificar configuraciones existentes.
- Eliminación de funcionalidades utilizadas por otras partes del sistema.

Ejemplo:

```
1.4.2 → 2.0.0
```

---

## MINOR

Se incrementa cuando se agregan nuevas funcionalidades compatibles con versiones anteriores.

Ejemplos:

- Nuevos collectors.
- Nuevos exportadores.
- Nuevas métricas.
- Nuevos módulos de monitoreo.
- Nuevas opciones de configuración.

Ejemplo:

```
1.4.2 → 1.5.0
```

---

## PATCH

Se incrementa cuando se corrigen errores sin modificar el comportamiento esperado para los usuarios.

Ejemplos:

- Corrección de bugs.
- Mejoras de estabilidad.
- Corrección de errores en cálculos.
- Ajustes menores en la documentación o configuración.

Ejemplo:

```
1.4.2 → 1.4.3
```

---

# Relación con Conventional Commits

ChuspaPulse utiliza los prefijos definidos en `docs/estilo-commits.md`.

La siguiente tabla resume la relación entre los tipos de commit y el versionado:

| Prefijo | Incremento de versión | Ejemplo |
|----------|-----------------------|----------|
| **feat** | MINOR | Nueva funcionalidad compatible |
| **fix** | PATCH | Corrección de un error |
| **docs** | No cambia la versión | Actualización de documentación |
| **test** | No cambia la versión | Nuevas pruebas |
| **refactor** | Normalmente no cambia la versión | Reorganización interna sin cambios funcionales |
| **chore** | Normalmente no cambia la versión | Tareas de mantenimiento |
| **security** | Generalmente PATCH | Corrección de una vulnerabilidad |
| **data** | Depende del impacto | Actualización de datos o recursos |

---

# Cambios incompatibles

Cuando un cambio rompe la compatibilidad con versiones anteriores, debe incrementarse la versión **MAJOR**, incluso si también incorpora nuevas funcionalidades o correcciones.

Ejemplos:

- Cambios incompatibles en la estructura de Storage.
- Eliminación de configuraciones existentes.
- Modificaciones incompatibles en la API HTTP.
- Eliminación de formatos de exportación soportados.

---

# Recomendaciones

Antes de publicar una nueva versión, se recomienda:

- Verificar que todas las pruebas pasen correctamente.
- Revisar que la documentación esté actualizada.
- Confirmar que el número de versión corresponda al tipo de cambios realizados.
- Registrar los cambios relevantes en las notas de la versión.

---

# Resumen

| Tipo de cambio | Nueva versión |
|----------------|---------------|
| Cambio incompatible | MAJOR |
| Nueva funcionalidad compatible | MINOR |
| Corrección de errores | PATCH |
