# Dashboard web embebido

## Introducción

ChuspaPulse incluye un dashboard web básico, servido desde el endpoint `GET /`, que ofrece una interfaz HTML sencilla para consultar el estado del sistema y acceder rápidamente a los principales endpoints de monitoreo, sin necesidad de herramientas externas.

Este dashboard funciona como punto de partida: los desarrolladores pueden personalizarlo o reemplazarlo por una interfaz más completa según la necesidad del proyecto.
---

# ¿Qué muestra el dashboard?

La página principal presenta información básica del sistema y enlaces rápidos a los endpoints disponibles del servicio.

Entre los elementos que puede mostrar se encuentran:

- Estado general del sistema.
- Enlaces a los endpoints HTTP disponibles.
- Actualización periódica de métricas mediante solicitudes `fetch`.
- Información de monitoreo obtenida desde el servidor.

El contenido se genera dinámicamente consultando los endpoints del servicio.

---

# Actualización en tiempo real

El dashboard utiliza la API Fetch de JavaScript para solicitar información al servidor sin necesidad de recargar la página.

De esta forma el usuario puede visualizar cambios en el estado del sistema de forma automática.

Un flujo simplificado es el siguiente:

```text
Navegador
      │
      ▼
GET /
      │
      ▼
Dashboard HTML
      │
      ▼
fetch()
      │
      ▼
Endpoints HTTP
      │
      ▼
Respuesta JSON
      │
      ▼
Actualización de la página
```

---

# Ejemplo de consulta

Un desarrollador puede consultar directamente un endpoint utilizando `curl`.

```bash
curl http://localhost:8080/metrics
```

La respuesta podría ser similar a:

```json
{
  "cpu": 18.4,
  "memoria": 43.7,
  "procesos": 132,
  "estado": "OK"
}
```

---

# Personalización del dashboard

El dashboard puede extenderse de distintas maneras.

Por ejemplo:

- Agregar nuevas tarjetas de información.
- Mostrar gráficos de CPU o memoria.
- Incorporar indicadores de alertas.
- Añadir tablas con procesos activos.
- Mostrar el historial de métricas.

También es posible modificar el intervalo de actualización para adaptarlo a las necesidades del sistema.

---

# Reemplazar el dashboard

Si una aplicación requiere una interfaz más avanzada, el dashboard embebido puede sustituirse por otra implementación.

Algunas opciones son:

- Integrar un frontend desarrollado con React.
- Utilizar Vue o Angular.
- Conectar un dashboard de Grafana.
- Crear una interfaz HTML personalizada.

Mientras los endpoints HTTP permanezcan compatibles, el resto del sistema no necesita modificarse.

---

# Recomendaciones

Se recomienda:

- Mantener los endpoints estables.
- Evitar consultas excesivamente frecuentes.
- Manejar errores de conexión en las llamadas `fetch`.
- Mostrar mensajes claros cuando el servidor no responda.

---

# Conclusión

El dashboard web embebido ofrece una forma sencilla de visualizar el estado del sistema desde un navegador. Su diseño, además, permite que cualquier desarrollador lo extienda o lo reemplace por una solución más completa sin necesidad de modificar la lógica principal de ChuspaPulse. 