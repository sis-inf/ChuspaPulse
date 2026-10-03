# Consideraciones y Guía de Seguridad en Pulso

Pulso es un monitor de infraestructura ligero que accede a información sensible del sistema desde `/proc` y expone un servidor HTTP para consultas. Por ello, es fundamental mantener un enfoque de mínimo privilegio y seguridad por diseño.

---

## 1. Modelo de Amenazas y Datos del Sistema

* **Acceso no autorizado:** Exposición de datos sobre carga, memoria, red y procesos a usuarios o hosts no permitidos.
* **Ataques de Denegación de Servicio (DoS):** Saturación de peticiones HTTP que reduzcan el rendimiento del servidor.
* **Inyección o lectura de archivos:** Lecturas indebidas mediante rutas relativas si no se restringen correctamente.
* **Mitigación:** Limitar la recolección únicamente a métricas necesarias y evitar registrar datos que comprometan la privacidad del usuario.

---

## 2. Permisos y Capacidades de Linux (Mínimo Privilegio)

* **No ejecutar como `root`:** Ejecutar Pulso con privilegios elevados amplía el impacto de cualquier fallo o vulnerabilidad. Se recomienda utilizar un usuario dedicado sin shell interactivo.
* **Capacidades de Linux:** Para leer `/proc` y las interfaces de disco o red, el agente solo requiere permisos de lectura estándar. **No requiere** capacidades administrativas como `CAP_SYS_ADMIN` ni `CAP_NET_ADMIN`.

---

## 3. Seguridad del Servidor HTTP

Actualmente el agente **no implementa autenticación HTTP** en sus endpoints (`/metrics`, `/health`, `/alerts`). Por ello, no debe exponerse a redes públicas sin capas de protección adicionales.

### Medidas de Mitigación Recomendadas:
1. **Vincular solo a `localhost`:** Configura el servidor para escuchar exclusivamente en la interfaz local si no requiere acceso remoto:
   ```toml
   [http]
   bind = "127.0.0.1"
   port = 8080