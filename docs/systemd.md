# Administración del servicio con systemd

Este documento describe cómo administrar ChuspaPulse como un servicio utilizando **systemd**. Se presentan los comandos básicos para iniciar, detener, habilitar y supervisar el servicio, además de la consulta de registros y recomendaciones para aplicar cambios en la configuración.

---

## Instalación del servicio

ChuspaPulse puede instalarse como un servicio del sistema mediante el mecanismo de instalación disponible para la distribución utilizada, ya sea mediante un paquete `.deb` o utilizando el subcomando `ChuspaPulse instalar-servicio`, cuando corresponda.

Una vez instalado el servicio, su administración puede realizarse con las herramientas estándar de **systemd**.

---

## Administración del servicio

### Iniciar el servicio

Para iniciar el servicio manualmente:

```bash
sudo systemctl start ChuspaPulse
```

### Detener el servicio

Para detener el servicio:

```bash
sudo systemctl stop ChuspaPulse
```

### Consultar el estado

Para verificar si el servicio se encuentra en ejecución:

```bash
sudo systemctl status ChuspaPulse
```

### Habilitar el inicio automático

Para configurar el inicio automático del servicio durante el arranque del sistema:

```bash
sudo systemctl enable ChuspaPulse
```

---

## Consulta de registros

Los registros del servicio pueden consultarse mediante `journalctl`.

Mostrar los registros almacenados:

```bash
sudo journalctl -u ChuspaPulse
```

Seguir los registros en tiempo real:

```bash
sudo journalctl -u ChuspaPulse -f
```

---

## Actualización de la configuración

Cuando se modifica la configuración del servicio, existen dos acciones habituales en **systemd**:

- **reload**: recarga la configuración sin reiniciar completamente el servicio, siempre que este admita esta operación.
- **restart**: detiene el servicio y lo inicia nuevamente para aplicar los cambios.

Si el servicio no admite la recarga mediante `reload`, utilice:

```bash
sudo systemctl restart ChuspaPulse
```

---

## Ejemplo de administración del servicio

El siguiente ejemplo muestra una secuencia básica para habilitar el servicio, iniciarlo y comprobar su estado:

```bash
sudo systemctl enable ChuspaPulse
sudo systemctl start ChuspaPulse
sudo systemctl status ChuspaPulse
```

---

## Conclusión

El uso de **systemd** facilita la administración de ChuspaPulse mediante comandos estándar para iniciar, detener, supervisar y consultar los registros del servicio, simplificando su operación en entornos Linux.
