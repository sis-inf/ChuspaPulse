# Seguridad en ChuspaPulse

ChuspaPulse es un monitor de infraestructura ligero que accede a información sensible del sistema (por ejemplo, desde `/proc`) y expone un servidor HTTP para consultas. Por ello, es fundamental mantener un enfoque de mínimo privilegio y seguridad por diseño.

Esta página es el punto de entrada de la documentación de seguridad: resume las consideraciones generales y enlaza a las guías específicas.

---

## Documentación de seguridad

| Documento | Para qué sirve | Estado en el código actual |
|---|---|---|
| [Checklist de hardening](seguridad-checklist.md) | Lista de verificación rápida antes de exponer ChuspaPulse fuera de `localhost` | Guía de revisión |
| [HTTPS/TLS](seguridad-tls.md) | Certificados y configuración de HTTPS | Configuración prevista; no implementada |
| [API key](seguridad-api-key.md) | Autenticación opcional de los endpoints HTTP | Configuración prevista; no implementada |
| [CORS](cors.md) | Orígenes permitidos para clientes web | Configuración prevista; no implementada |

> **Nota:** En el código actual (`src/`) no se leen las claves `api_key`, `tls_cert_path`, `tls_key_path` ni `cors_origin`, y no existe limitación de tasa de peticiones. Las guías de TLS, API key y CORS describen la configuración prevista. Hasta que se implementen, aplica lo indicado en la [sección 7](#7-seguridad-del-servidor-http).

---

## 1. Acceso a datos del sistema

- **Riesgo:** ChuspaPulse accede a información sensible como uso de CPU, memoria, procesos y red. Un mal manejo podría exponer datos internos del sistema o del usuario. También existe riesgo de lecturas indebidas mediante rutas relativas si no se restringen correctamente.
- **Mitigación:**
  - Limitar la recolección de datos únicamente a lo necesario.
  - Evitar almacenar información sensible innecesaria.
  - No registrar datos que puedan comprometer la privacidad del usuario.

---

## 2. Permisos de ejecución

- **Riesgo:** Ejecutar ChuspaPulse con privilegios elevados (root/administrador) aumenta el impacto de cualquier fallo o vulnerabilidad.
- **Mitigación:**
  - **No ejecutar como `root`.** Se recomienda utilizar un usuario dedicado sin shell interactivo.
  - Diseñar el sistema para funcionar con privilegios mínimos y solicitar permisos elevados solo cuando sea estrictamente necesario.
  - **Capacidades de Linux:** para leer `/proc` y las interfaces de disco o red, el agente solo requiere permisos de lectura estándar. **No requiere** capacidades administrativas como `CAP_SYS_ADMIN` ni `CAP_NET_ADMIN`.
  - Documentar claramente los permisos requeridos.

---

## 3. Exposición de información

- **Riesgo:** Mostrar métricas del sistema sin control puede exponer información crítica, especialmente si se integra con interfaces web o APIs.
- **Mitigación:**
  - Evitar exponer datos sensibles en interfaces públicas.
  - Implementar mecanismos de control de acceso (ver [API key](seguridad-api-key.md) y [CORS](cors.md)).

---

## 4. Manejo de errores

- **Riesgo:** Mensajes de error detallados pueden revelar información interna del sistema, como rutas, configuraciones o fallos específicos.
- **Mitigación:**
  - Mostrar mensajes de error genéricos al usuario final.
  - Registrar detalles técnicos solo en logs internos controlados.
  - Evitar exponer stack traces en producción.

---

## 5. Consumo de recursos y denegación de servicio

- **Riesgo:** Un monitoreo demasiado frecuente puede consumir recursos excesivos y afectar el rendimiento del sistema. Además, un volumen elevado de peticiones HTTP puede saturar el servidor.
- **Mitigación:**
  - Definir frecuencias de muestreo razonables (`intervalo_segundos` en la sección `[sampler]` de `pulso.toml`).
  - Optimizar el uso de CPU y memoria.
  - Limitar el acceso al puerto HTTP (ver [sección 7](#7-seguridad-del-servidor-http)).

---

## 6. Compatibilidad y diferencias entre sistemas operativos

- **Riesgo:** Diferencias entre Linux y Windows pueden generar comportamientos inesperados o vulnerabilidades si no se manejan correctamente.
- **Mitigación:**
  - Implementar capas de abstracción para cada sistema operativo.
  - Validar el comportamiento en ambos entornos.
  - Evitar dependencias inseguras o específicas sin validación.

---

## 7. Seguridad del servidor HTTP

Actualmente el agente **no implementa autenticación HTTP** en sus endpoints (`/metrics`, `/health`, `/alerts`). Cualquier cliente con acceso a la red puede consultar métricas internas, estados y configuración. Por ello, no debe exponerse a redes públicas sin capas de protección adicionales.

### Amenazas específicas

1. **Acceso no autorizado a métricas y datos**, por la ausencia de autenticación y autorización.
2. **Denegación de servicio (DoS)**: un atacante podría enviar un volumen elevado de peticiones para saturar el servidor.

### Medidas de mitigación recomendadas

1. **Vincular el servidor solo a `localhost`** si no se necesita acceso remoto, en `pulso.toml`:

   ```toml
   [servidor]
   host = "127.0.0.1"
   puerto = 8080
   ```

2. **Proteger el puerto con un cortafuegos (firewall):**
   - Permitir conexiones solo desde IPs confiables o dentro de la red local.
   - Bloquear intentos de acceso desde direcciones desconocidas.
3. **Control de tasa de peticiones (mejora futura):** se planea implementar limitación de velocidad para evitar abusos y ataques DoS.

### Guías relacionadas

- [HTTPS/TLS](seguridad-tls.md): cifrado de las comunicaciones.
- [API key](seguridad-api-key.md): autenticación de los endpoints.
- [CORS](cors.md): orígenes permitidos para clientes web.
- [Checklist de hardening](seguridad-checklist.md): verificación previa a la exposición.

> **Importante:** En la versión actual no existe autenticación para los endpoints HTTP. No se recomienda exponer el servicio directamente a internet público sin capas adicionales de seguridad.

---

## Notas finales

- La seguridad es un proceso continuo y debe revisarse constantemente.
- Estas consideraciones pueden ampliarse a medida que el proyecto evolucione.
- ChuspaPulse prioriza un enfoque seguro sin comprometer su ligereza y rendimiento.
