# Seguridad mediante API key

## Introducción

ChuspaPulse permite habilitar una autenticación opcional mediante una API key para proteger los endpoints HTTP. Cuando esta opción está configurada, los clientes deben autenticarse enviando la clave correspondiente en cada solicitud.

## Configuración

Configure la API key en el archivo `ChuspaPulse.toml`:

```toml
[servidor]
api_key = "mi_api_key_segura"
```

Reemplace `"mi_api_key_segura"` por la clave que desee utilizar.

## Autenticación

Los clientes deben enviar el siguiente encabezado HTTP en cada solicitud protegida:

```http
Authorization: Bearer <api_key>
```

Donde `<api_key>` corresponde al valor configurado en `ChuspaPulse.toml`.

## Endpoints exceptuados

El siguiente endpoint permanece accesible sin autenticación:

* `/health`

Esto permite realizar verificaciones del estado del servicio incluso cuando la autenticación mediante API key está habilitada.

## Ejemplo de petición autenticada

```bash
curl -H "Authorization: Bearer mi_api_key_segura" http://localhost:8080/metrics
```
