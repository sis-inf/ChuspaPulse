# Caso de Prueba: Fuzzing HTTP
## Objetivo: Verificar la robustez de los endpoints HTTP frente a payloads malformados y verificar la configuración de headers de seguridad.Precondiciones:
- pulso corriendo con el servidor HTTP habilitado.
- Herramienta de fuzzing instalada (ej. curl, wfuzz o Burp Suite).
## Pasos:
1. Enviar una petición GET a /metrics con payloads extremadamente largos en los parámetros de consulta.
2. Enviar peticiones con caracteres especiales y formatos inesperados (ej. JSON en un campo de texto plano).
3. Revisar los headers de respuesta HTTP (X-Frame-Options, Content-Type, etc.).
## Criterio de Éxito:
- El servidor no crashea (exit code 0) ni se cuelga ante inputs malformados.
- El servidor responde con un código de error apropiado (ej. 400 Bad Request) en lugar de 500 Internal Server Error.
- Los headers de seguridad están presentes y configurados correctamente.