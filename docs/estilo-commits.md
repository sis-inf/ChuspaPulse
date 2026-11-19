# Estilo de Commits para ChuspaPulse

## Introducción

El proyecto ChuspaPulse utiliza Conventional Commits para mantener un historial claro y consistente.

## Formato

Todos los commits deben seguir el formato:

text
tipo(ambito): descripción breve


Ejemplo:

text
feat(collectors): agregar colector de métricas de CPU


---

## Tipos permitidos

| Tipo     | Descripción              |
| -------- | ------------------------ |
| feat     | Nueva funcionalidad      |
| fix      | Corrección de errores    |
| docs     | Documentación            |
| chore    | Tareas de mantenimiento  |
| test     | Pruebas                  |
| refactor | Reestructuración interna |
| perf     | Mejoras de rendimiento   |
| ci       | Integración continua     |

---

## Ámbitos recomendados para ChuspaPulse

| Ámbito     | Uso                           |
| ---------- | ----------------------------- |
| collectors | Recolección de métricas       |
| formatters | Formateadores de salida       |
| http       | Comunicación HTTP             |
| storage    | Almacenamiento de datos       |
| alertas    | Sistema de alertas            |
| cli        | Interfaz de línea de comandos |
| config     | Configuración                 |
| cmake      | Configuración CMake           |
| ci         | Integración continua          |
| tests      | Pruebas automatizadas         |

---

## Ejemplos correctos

1. feat(collectors): agregar monitoreo de memoria
2. feat(http): implementar cliente HTTP
3. fix(storage): corregir escritura de archivos
4. fix(cli): validar parámetros de entrada
5. chore(cmake): actualizar configuración de compilación
6. docs(docs): agregar guía de instalación
7. test(collectors): añadir pruebas de CPU
8. perf(formatters): optimizar generación de reportes

---

## Ejemplos incorrectos

### Incorrecto

text
arreglos


### Correcto

text
fix(storage): corregir acceso a archivos


### Incorrecto

text
nuevo codigo


### Correcto

text
feat(http): implementar descarga de métricas


### Incorrecto

text
documentacion


### Correcto

text
docs(docs): actualizar README


### Incorrecto

text
cambios varios


### Correcto

text
refactor(cli): reorganizar comandos


### Incorrecto

text
test


### Correcto

text
test(storage): agregar pruebas unitarias


---

## Recomendaciones

* Utilizar siempre Conventional Commits.
* Seleccionar el ámbito adecuado.
* Mantener descripciones breves y claras.
* Evitar mensajes genéricos.
* Escribir la descripción en infinitivo.
*
