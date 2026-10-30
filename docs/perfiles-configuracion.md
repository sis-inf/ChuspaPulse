# Perfiles de configuración

## Resumen

Pulso permite definir múltiples perfiles de configuración dentro del archivo `ChuspaPulse.toml`. Los perfiles facilitan utilizar diferentes configuraciones para distintos entornos, como desarrollo y producción, sin necesidad de mantener varios archivos de configuración.

Cada perfil puede seleccionarse al iniciar la aplicación mediante el parámetro `--perfil`.

---

# Estructura del archivo TOML

La configuración general se define en la sección raíz del archivo.

Los perfiles adicionales se crean utilizando la siguiente sintaxis:

```toml
[perfiles.nombre]
```

Donde `nombre` corresponde al identificador del perfil.

Por ejemplo:

```toml
[perfiles.desarrollo]

[perfiles.produccion]
```

---

# Funcionamiento de los perfiles

Al iniciar ChuspaPulse, primero se carga la configuración definida en la sección raíz del archivo.

Si se selecciona un perfil mediante `--perfil`, los valores definidos dentro de ese perfil reemplazan únicamente los parámetros correspondientes, manteniendo el resto de la configuración original.

De esta forma es posible reutilizar una configuración base y modificar solamente los parámetros necesarios para cada entorno.

---

# Seleccionar un perfil

El perfil puede indicarse al ejecutar ChuspaPulse mediante el parámetro:

```bash
ChuspaPulse --config pulso.toml --perfil desarrollo
```

o bien:

```bash
ChuspaPulse --config pulso.toml --perfil produccion
```

---

# Ejemplo completo

A continuación se muestra un ejemplo de un archivo `ChuspaPulse.toml` con dos perfiles.

```toml
intervalo = 5
retencion_dias = 30
host = "localhost"

[perfiles.desarrollo]
intervalo = 2
retencion_dias = 7

[perfiles.produccion]
intervalo = 30
retencion_dias = 180
host = "servidor-produccion"
```

En este ejemplo:

- La configuración raíz define los valores por defecto.
- El perfil `desarrollo` reduce el intervalo de muestreo y la retención de datos.
- El perfil `produccion` incrementa la retención y utiliza un host diferente.

---

# Casos de uso

## Desarrollo

El perfil de desarrollo suele utilizar:

- Intervalos de muestreo cortos.
- Menor retención de datos.
- Configuración orientada a pruebas.

## Producción

El perfil de producción normalmente utiliza:

- Intervalos de muestreo más estables.
- Mayor retención de métricas.
- Configuración preparada para uso continuo.

---

# Buenas prácticas

Se recomienda:

- Mantener una configuración base con los valores comunes.
- Definir únicamente las diferencias dentro de cada perfil.
- Utilizar nombres descriptivos para los perfiles.
- Verificar el perfil seleccionado antes de iniciar la aplicación.

---

# Conclusiones

Los perfiles de configuración permiten reutilizar un único archivo `ChuspaPulse.toml` para distintos escenarios de ejecución.

Gracias al parámetro `--perfil`, es posible adaptar el comportamiento de ChuspaPulse para entornos de desarrollo y producción sin duplicar configuraciones, facilitando el mantenimiento y reduciendo errores de configuración.