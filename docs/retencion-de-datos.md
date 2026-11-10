# Política de retención y purga automática de datos

## Introducción

ChuspaPulse permite configurar una política de retención para controlar cuánto tiempo se conservan los snapshots almacenados. Esta funcionalidad ayuda a administrar el espacio en disco eliminando automáticamente la información más antigua cuando supera el período configurado.

> **Advertencia:** la purga automática elimina los datos de forma permanente. Una vez eliminados los snapshots, no es posible recuperarlos desde ChuspaPulse.

---

# Configuración de la retención

La política de retención se configura mediante el parámetro `retencion_dias` en el archivo `pulso.toml`.

Ejemplo:

```toml
retencion_dias = 30
```

En este caso, ChuspaPulse conservará los snapshots de los últimos 30 días y eliminará automáticamente aquellos cuya antigüedad sea mayor.

---

# Retención infinita

Si se establece:

```toml
retencion_dias = 0
```

ChuspaPulse conservará todos los snapshots sin eliminarlos automáticamente.

Este es el comportamiento de **retención infinita** y corresponde al valor por defecto cuando no se desea aplicar una política de purga.

---

# Funcionamiento de la purga automática

Durante la ejecución del sistema, ChuspaPulse revisa periódicamente la antigüedad de los snapshots almacenados.

Si encuentra registros cuya antigüedad supera el valor configurado en `retencion_dias`, estos se eliminan automáticamente para liberar espacio.

Este mecanismo permite:

- Reducir el uso del almacenamiento.
- Mantener la base de datos organizada.
- Evitar el crecimiento indefinido del historial de snapshots.

---

# Advertencia importante

> **La purga automática es irreversible.**

Antes de disminuir el valor de `retencion_dias`, se recomienda realizar una copia de seguridad de los datos que sea necesario conservar.

---

# Ejemplos de configuración

Conservar los datos durante siete días:

```toml
retencion_dias = 7
```

Conservar los datos durante noventa días:

```toml
retencion_dias = 90
```

Conservar todos los snapshots:

```toml
retencion_dias = 0
```

---

# Recomendaciones

- Utilizar valores bajos cuando el sistema genere un gran volumen de datos.
- Configurar un período de retención acorde a las necesidades de monitoreo.
- Utilizar `retencion_dias = 0` únicamente cuando exista suficiente espacio de almacenamiento o se requiera conservar todo el historial.
- Realizar copias de seguridad antes de modificar la política de retención.