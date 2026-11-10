# Collector de Docker

## Resumen

ChuspaPulse incluye un collector capaz de detectar contenedores Docker en ejecución mediante la consulta del socket Unix de Docker.

Este collector permite obtener métricas generales sobre el estado de los contenedores disponibles en el host, proporcionando una visión rápida del entorno Docker sin necesidad de instalar componentes adicionales.

---

# Prerrequisitos

Para que el collector funcione correctamente, el usuario que ejecuta ChuspaPulse debe tener permisos para acceder al socket de Docker.

La ubicación habitual del socket es:

```text
/var/run/docker.sock
```

En la mayoría de las distribuciones Linux, esto implica que el usuario pertenezca al grupo:

```text
docker
```

Puede verificarse con:

```bash
groups
```

Si el grupo `docker` aparece en la lista, el usuario dispone de los permisos necesarios para acceder al socket.

---

# Advertencia de seguridad

Pertenecer al grupo `docker` concede permisos elevados sobre el sistema.

En la práctica, un usuario con acceso al socket de Docker puede realizar acciones equivalentes a privilegios de administrador (root), como:

- Crear y eliminar contenedores.
- Montar directorios del sistema.
- Ejecutar procesos con privilegios elevados.

Por este motivo, solo deben agregarse al grupo `docker` usuarios de confianza.

---

# Métricas disponibles

El collector expone actualmente las siguientes métricas:

| Métrica | Descripción |
|---------|-------------|
| `docker.containers_running` | Número de contenedores actualmente en ejecución. |
| `docker.containers_total` | Número total de contenedores detectados por Docker. |

Estas métricas permiten conocer rápidamente el estado general del entorno Docker monitorizado.

---

# Ejemplo

Un sistema con cinco contenedores registrados y tres en ejecución podría generar información similar a:

```text
docker.containers_running = 3

docker.containers_total = 5
```

---

# Limitaciones actuales

Actualmente este collector proporciona únicamente métricas agregadas.

No expone información individual de cada contenedor, como por ejemplo:

- Nombre del contenedor.
- Estado individual.
- Uso de CPU.
- Uso de memoria.
- Estadísticas de red.
- Estadísticas de almacenamiento.

Su propósito es ofrecer una visión general del entorno Docker.

---

# ¿Qué ocurre si Docker no está disponible?

Si Docker no está instalado, el servicio no está en ejecución o el usuario no tiene permisos para acceder al socket, el collector no podrá obtener información.

En estos casos es recomendable comprobar:

```bash
ls -l /var/run/docker.sock
```

y verificar que:

- El archivo existe.
- El servicio Docker está iniciado.
- El usuario dispone de permisos suficientes para acceder al socket.

---

# Conclusiones

El collector de Docker permite conocer el número de contenedores disponibles y en ejecución mediante la consulta del socket Unix de Docker.

Para utilizarlo es necesario contar con permisos de acceso al socket, teniendo presente que dichos permisos implican privilegios elevados sobre el sistema. Actualmente, el collector ofrece únicamente métricas agregadas y no proporciona información detallada por contenedor individual.