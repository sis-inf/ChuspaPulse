# Política de CHANGELOG

Este documento establece la política para definir qué cambios deben registrarse en `CHANGELOG.md`, cuáles generan una entrada visible para los usuarios y cuáles corresponden únicamente a tareas internas de desarrollo.

---

## Objetivo

El archivo `CHANGELOG.md` contiene un resumen de los cambios importantes realizados en cada versión de ChuspaPulse.

La finalidad del changelog es permitir que los usuarios conozcan nuevas funcionalidades, correcciones y cambios importantes sin revisar todos los commits del repositorio.

ChuspaPulse utiliza la convención **Conventional Commits** para identificar qué cambios generan entradas en el changelog.

El formato utilizado para los commits es:

```text
tipo(alcance): descripción del cambio
```

Ejemplo:

```text
feat(http): agregar endpoint de información del sistema
```

---

## Cambios que generan entrada en CHANGELOG.md

### feat — Nuevas funcionalidades

Los commits con tipo `feat` generan una entrada en el changelog porque representan nuevas características o capacidades del sistema.

Ejemplos:

- Nuevo recolector de métricas.
- Nuevo endpoint HTTP.
- Nuevo formato de salida.

Ejemplo de commit:

```text
feat(http): agregar endpoint /version
```

Estos cambios se registran normalmente en la sección:

```markdown
### Añadido
```

---

### fix — Correcciones de errores

Los commits con tipo `fix` generan entradas porque solucionan errores que afectan el funcionamiento del sistema.

Ejemplos:

- Corrección de errores en almacenamiento.
- Solución de fallos en endpoints.
- Corrección de problemas de configuración.

Ejemplo de commit:

```text
fix(storage): corregir error al guardar métricas
```

Estos cambios se registran normalmente en la sección:

```markdown
### Corregido
```

---

### Breaking changes — Cambios incompatibles

Los cambios que rompen compatibilidad con versiones anteriores deben registrarse siempre.

Se pueden identificar utilizando `!` en el commit:

```text
feat(api)!: cambiar formato de respuesta del endpoint
```

También pueden declararse mediante:

```text
BREAKING CHANGE:
Se modificó la estructura del archivo de configuración.
```

Estos cambios deben explicar claramente qué debe hacer el usuario al actualizar la versión.

Ejemplo:

```markdown
### Cambios incompatibles

- Se modificó la estructura del archivo de configuración.
- Se requiere actualizar configuraciones existentes.
```

---

## Cambios excluidos por defecto

Algunos tipos de commits corresponden a tareas internas del desarrollo y no generan entradas en `CHANGELOG.md`, excepto cuando tienen impacto visible para los usuarios.

---

### docs — Documentación

Los commits de documentación no generan entradas normalmente.

Ejemplos:

```text
docs: actualizar documentación del proyecto
```

```text
docs: crear guía de instalación
```

Se incluyen únicamente cuando la documentación representa un cambio importante para el usuario.

Ejemplo:

```text
docs: documentar nuevo formato obligatorio de configuración
```

---

### chore — Mantenimiento interno

Los commits `chore` no generan entradas porque generalmente corresponden a mantenimiento del proyecto.

Ejemplos:

```text
chore: actualizar dependencias
```

```text
chore: reorganizar archivos internos
```

Solo deben registrarse si modifican el comportamiento visible del sistema.

---

### test — Pruebas

Los commits relacionados con pruebas no generan entradas en el changelog.

Ejemplos:

```text
test: agregar pruebas del collector CPU
```

```text
test: mejorar cobertura del módulo storage
```

Estos cambios ayudan al desarrollo y calidad del proyecto, pero no representan cambios para usuarios finales.

---

### refactor — Mejoras internas

Los commits de tipo `refactor` no generan entradas si no modifican el comportamiento del sistema.

Ejemplo:

```text
refactor(core): reorganizar arquitectura interna
```

Si el cambio solamente mejora la estructura interna del código, no debe aparecer en el changelog.

---

## Formato esperado de las entradas

Las entradas del changelog deben ser breves, claras y enfocadas en el usuario.

Ejemplo:

```markdown
### Añadido

- Agregado soporte para recolectar métricas de temperatura.

### Corregido

- Corregido error al iniciar ChuspaPulse con configuraciones inválidas.
```

Cada entrada debe:

- Explicar qué cambió.
- Evitar detalles internos innecesarios.
- Indicar el beneficio o impacto para el usuario cuando sea posible.

---

## Ejemplo práctico

Supongamos los siguientes commits:

```text
feat(http): agregar endpoint /version

fix(storage): corregir consulta de historial

docs: actualizar documentación

test: agregar pruebas del formatter JSON

chore: actualizar dependencias
```

El resultado esperado en `CHANGELOG.md` sería:

```markdown
## Unreleased

### Añadido

- Agregado endpoint HTTP /version.

### Corregido

- Corregido problema al consultar historial de métricas.
```

Los cambios de documentación, pruebas y mantenimiento no aparecen porque no afectan directamente al usuario.

---

## Resumen de política

| Tipo de commit | ¿Aparece en CHANGELOG.md? |
|----------------|---------------------------|
| `feat` | Sí |
| `fix` | Sí |
| `breaking change` | Sí |
| `docs` | No, salvo impacto visible |
| `chore` | No, salvo impacto visible |
| `test` | No |
| `refactor` | No, salvo cambio observable |

Esta política permite mantener un `CHANGELOG.md` ordenado, útil para usuarios y coherente con la generación automática basada en Conventional Commits.
