# Checklist de Hardening de Seguridad

> Este documento es una lista de verificación rápida para asegurar ChuspaPulse antes de
> exponerlo fuera de localhost. Para la política de seguridad completa, consulta
> [docs/seguridad.md](seguridad.md).

---

## ¿Cómo usar este checklist?

Revisa cada ítem antes de desplegar ChuspaPulse en un entorno accesible desde la red.
Cada ítem enlaza al documento detallado con instrucciones de configuración.

---

## Checklist

### 🔒 TLS

- [ ] TLS está activado si el servicio es accesible fuera de localhost.
- [ ] El certificado es válido y no está vencido.
- [ ] Se usa TLS 1.2 o superior.

> Ver configuración detallada: [docs/seguridad-tls.md](seguridad-tls.md)

---

### 🔑 API Key

- [ ] La API key está configurada y es obligatoria para acceder a los endpoints.
- [ ] La API key no está hardcodeada en el código fuente.
- [ ] La API key tiene al menos 32 caracteres aleatorios.

> Ver configuración detallada: [docs/seguridad-api-key.md](seguridad-api-key.md)

---

### 🚦 Rate Limiting

- [ ] El rate limiting está configurado si el servicio está expuesto públicamente.
- [ ] Se define un límite razonable de peticiones por minuto por cliente.
- [ ] Se retorna HTTP 429 cuando se supera el límite.

> Ver configuración detallada: [docs/seguridad.md](seguridad.md#7-seguridad-del-servidor-http)

---

### 🌐 CORS

- [ ] CORS está restringido a orígenes específicos conocidos (no `"*"`).
- [ ] No se permiten orígenes comodín en producción.
- [ ] Solo se permiten los métodos HTTP necesarios.

> Ver configuración detallada: [docs/seguridad-api-key.md](seguridad-api-key.md)

---

### 👤 Permisos del proceso

- [ ] ChuspaPulse no corre como root salvo que un collector específico lo requiera.
- [ ] El colector de batería puede correr sin privilegios elevados.
- [ ] Se usan los permisos mínimos necesarios para cada collector.
- [ ] Los permisos requeridos están documentados.

> Ver configuración detallada: [docs/seguridad.md](seguridad.md#2-permisos-de-ejecución)

---

### 🔌 Exposición de red

- [ ] El servidor HTTP está vinculado a `127.0.0.1` si no se necesita acceso externo.
- [ ] El puerto no está expuesto públicamente sin firewall.
- [ ] Solo las IPs confiables tienen acceso al puerto de ChuspaPulse.

```toml
[servidor]
host = "127.0.0.1"
puerto = 8080
```

> Ver configuración detallada: [docs/seguridad.md](seguridad.md#7-seguridad-del-servidor-http)

---

### 📋 Logs y errores

- [ ] Los mensajes de error no exponen rutas internas ni stack traces.
- [ ] Los logs detallados solo se escriben en archivos internos controlados.
- [ ] El nivel de log en producción es `info` o superior (no `debug`).

```toml
[storage]
nivel_log = "info"
```

> Ver configuración detallada: [docs/seguridad.md](seguridad.md#4-manejo-de-errores)

---

## Resumen rápido

| Ítem | Obligatorio si... |
|------|------------------|
| TLS | Accesible fuera de localhost |
| API Key | Expuesto en red local o pública |
| Rate Limiting | Expuesto públicamente |
| CORS | Consume desde un navegador |
| Permisos mínimos | Siempre |
| Bind a localhost | No se necesita acceso externo |
| Logs controlados | Siempre |