# Configuración HTTPS/TLS

Este documento describe cómo generar un certificado autofirmado para pruebas locales, un ejemplo de configuración de HTTPS/TLS en `ChuspaPulse.toml` y recomendaciones para el uso de certificados en entornos de producción.

---

## 1. Generar un certificado autofirmado para pruebas locales

Para realizar pruebas locales puede utilizar un certificado autofirmado generado con **OpenSSL**.

Ejecute el siguiente comando:

```bash
openssl req -x509 \
  -newkey rsa:2048 \
  -keyout key.pem \
  -out cert.pem \
  -days 365 \
  -nodes \
  -subj "/CN=localhost"
```

Este comando genera los siguientes archivos:

* `cert.pem`: certificado autofirmado.
* `key.pem`: clave privada asociada al certificado.

> **Nota:** Los certificados autofirmados son adecuados para pruebas locales. Para entornos de producción se recomienda utilizar certificados emitidos por una autoridad de certificación confiable.

---

## 2. Configurar HTTPS/TLS en `ChuspaPulse.toml`

Una vez disponibles el certificado y la clave privada, configure las rutas correspondientes en el archivo `ChuspaPulse.toml`.

Ejemplo de configuración:

```toml
[servidor]
host = "0.0.0.0"
puerto = 8080

tls_cert_path = "/ruta/al/certificado/cert.pem"
tls_key_path = "/ruta/a/la/clave/key.pem"
```

> **Nota:** Sustituya las rutas de ejemplo por la ubicación real del certificado y la clave privada en su sistema.

---

## 3. Recomendaciones para entornos de producción

Para entornos de producción se recomienda utilizar certificados emitidos por una autoridad de certificación confiable.

Las opciones más comunes son:

* Utilizar **Let's Encrypt** junto con un proxy reverso para administrar y renovar automáticamente los certificados.
* Utilizar certificados emitidos por una autoridad certificadora interna cuando la infraestructura pertenezca a una red privada.

Evite utilizar certificados autofirmados en entornos de producción, ya que los clientes no podrán verificar automáticamente su autenticidad.

---

## 4. Buenas prácticas de seguridad

* Proteja la clave privada y limite el acceso únicamente a los procesos autorizados.
* Utilice versiones de TLS 1.2 o superiores.
* Renueve los certificados antes de su fecha de expiración.
* Verifique que las rutas configuradas para el certificado y la clave privada sean correctas antes de iniciar el servicio.
