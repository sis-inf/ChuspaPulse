# Checklist de Producción

Lista de verificación accionable antes de desplegar ChuspaPulse en un servidor real.
Revisa cada ítem y márcalo antes de exponer el agente en producción.

---

## 🔑 Autenticación

- [ ] Configuraste una API key o algún mecanismo de autenticación para los endpoints HTTP.
- [ ] La API key tiene al menos 32 caracteres aleatorios.
- [ ] La API key no está hardcodeada en el código fuente ni en variables de entorno públicas.

> Ver: [docs/seguridad-checklist.md](seguridad-checklist.md)

---

## 🔒 TLS

- [ ] TLS está activado si el agente es accesible fuera de localhost.
- [ ] El certificado es válido y no está vencido.
- [ ] El servidor no acepta conexiones HTTP planas en producción.

```toml
[servidor]
host = "0.0.0.0"
puerto = 8080
```

> Ver: [docs/seguridad-checklist.md](seguridad-checklist.md)

---

## 🗄️ Retención de datos

- [ ] Configuraste `retencion_dias` para evitar crecimiento ilimitado de la base de datos.
- [ ] Verificaste el espacio disponible en disco antes de desplegar.
- [ ] Tienes un proceso de backup periódico de `chuspapulse.db`.

```toml
[storage]
ruta_db = "chuspapulse.db"
retencion_dias = 30
```

> Ver: [docs/guia-migracion-esquema-bd.md](guia-migracion-esquema-bd.md)

---

## ✅ Verificación de prerrequisitos

- [ ] Ejecutaste `--check` para verificar que todos los prerrequisitos están satisfechos.
- [ ] No hay errores ni advertencias en la salida de `--check`.

```bash
./chuspapulse --check
```

---

## ⚠️ Alertas

- [ ] Configuraste alertas para las métricas críticas de tu caso de uso.
- [ ] Verificaste que las alertas se disparan correctamente con valores de prueba.
- [ ] Los umbrales de alerta son apropiados para tu entorno (no demasiado sensibles ni demasiado permisivos).

> Ver: [docs/alertas.md](alertas.md)

---

## 🔄 Arranque automático con systemd

- [ ] Instalaste ChuspaPulse como servicio systemd para reinicio automático ante fallos.
- [ ] El servicio está habilitado para arrancar con el sistema.
- [ ] Verificaste que el servicio arranca correctamente tras un reinicio.

```ini
# /etc/systemd/system/chuspapulse.service
[Unit]
Description=ChuspaPulse - Monitor de infraestructura
After=network.target

[Service]
Type=simple
User=chuspapulse
WorkingDirectory=/opt/chuspapulse
ExecStart=/opt/chuspapulse/chuspapulse --daemon
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
```

```bash
# Habilitar e iniciar el servicio
sudo systemctl daemon-reload
sudo systemctl enable chuspapulse
sudo systemctl start chuspapulse

# Verificar que está corriendo
sudo systemctl status chuspapulse
```

---

## 🌐 Red y firewall

- [ ] El puerto de ChuspaPulse está restringido por firewall a IPs confiables.
- [ ] El servidor no está expuesto directamente a internet sin protección adicional.
- [ ] Verificaste que el puerto configurado no está en uso por otro proceso.

```bash
# Verificar que el puerto está disponible
ss -tlnp | grep 8080
```

---

## 📋 Logs

- [ ] El nivel de log está configurado como `info` o superior (no `debug`) en producción.
- [ ] Los logs se están escribiendo correctamente.
- [ ] Tienes un proceso de rotación de logs configurado.

```toml
[storage]
nivel_log = "info"
```

---

## Resumen rápido

| Área | Obligatorio |
|------|------------|
| API key / autenticación | Sí, si es accesible en red |
| TLS | Sí, si es accesible fuera de localhost |
| Retención de datos | Sí, siempre |
| `--check` | Sí, antes de cada despliegue |
| Alertas | Recomendado |
| systemd | Recomendado para producción |
| Firewall | Sí, si es accesible en red |
| Logs en `info` | Sí, siempre |