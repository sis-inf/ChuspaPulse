# Proceso de publicación de una nueva versión

## Resumen

El proceso de publicación (release) de ChuspaPulse consiste en preparar una nueva versión del proyecto, actualizar la documentación correspondiente y generar los artefactos de distribución para los usuarios.

Este flujo busca garantizar que cada versión publicada sea consistente, trazable y reproducible.

---

# Flujo general del proceso

El proceso recomendado es el siguiente:

1. Decidir que existe una nueva versión lista para publicarse.
2. Actualizar el número de versión en `CMakeLists.txt`.
3. Revisar y actualizar el `CHANGELOG`.
4. Crear un tag de Git para la versión.
5. Ejecutar el pipeline de CI/CD.
6. Publicar los artefactos generados.

---

# Actualizar la versión

ChuspaPulse utiliza la versión definida dentro de `CMakeLists.txt`.

Cuando se prepara una nueva versión debe actualizarse el parámetro `VERSION` del proyecto siguiendo la política de versionado documentada en:

```
docs/versionado.md
```

Ejemplo:

```cmake
project(
    ChuspaPulse
    VERSION 1.4.0
)
```

---

# Revisar el CHANGELOG

Antes de crear el release debe verificarse que el archivo `CHANGELOG.md` contenga:

- Nuevas funcionalidades.
- Correcciones de errores.
- Cambios incompatibles (si existen).
- Mejoras de rendimiento.
- Actualizaciones de documentación.

El CHANGELOG permite conocer las diferencias entre versiones.

---

# Crear el tag

Una vez actualizada la versión puede crearse un tag de Git.

Ejemplo:

```bash
git tag v1.4.0
git push origin v1.4.0
```

El tag identifica exactamente el código fuente correspondiente a la versión publicada.

---

# Pipeline de CI/CD

Después de crear el tag puede ejecutarse el pipeline de publicación configurado por el proyecto.

El flujo de despliegue se encuentra definido en:

```
.github/workflows/deploy.yml
```

Dependiendo de la configuración del repositorio, este pipeline puede:

- Compilar ChuspaPulse.
- Ejecutar pruebas.
- Construir la imagen Docker.
- Generar paquetes binarios.
- Publicar los artefactos.

---

# Artefactos publicados

Los artefactos generados pueden incluir:

- Imagen Docker.
- Paquetes `.deb`.
- Binarios compilados.
- Archivos asociados al release.

Los artefactos disponibles dependen de la configuración del pipeline de despliegue.

---

# Buenas prácticas

Antes de publicar una nueva versión se recomienda:

- Verificar que todas las pruebas finalicen correctamente.
- Confirmar que la versión fue actualizada.
- Revisar el CHANGELOG.
- Confirmar que la documentación se encuentre actualizada.
- Etiquetar únicamente versiones estables.

---

# Referencias

- `docs/versionado.md`
- `CHANGELOG.md`
- `CMakeLists.txt`
- `.github/workflows/deploy.yml`