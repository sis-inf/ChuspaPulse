# Casos de Prueba Funcionales

| ID | Requerimiento Asociado | Descripción del Caso | Precondiciones | Pasos | Resultado Esperado |
|---|---|---|---|---|---|
| CP-001 | RF-001 | Validar monitoreo de disponibilidad mediante ping y chequeos HTTP. | Servidor activo y accesible. | 1. Ejecutar monitoreo.<br>2. Verificar respuesta de ping/HTTP. | El sistema muestra estado “Disponible” si responde correctamente. |
| CP-002 | RF-002 | Validar recolección y visualización de métricas CPU, memoria y red. | Servidor con agente instalado. |  1. Iniciar agente.<br>2. Consultar panel de métricas. | El sistema muestra valores en tiempo real de CPU, memoria y red.  |
| CP-003 | RF-003 | Validar generación de alertas ante caída de servicio. | Servicio configurado con umbral crítico. | 1. Simular caída del servicio.<br>2. Revisar alertas. | El sistema genera alerta inmediata indicando el fallo. |
| CP-004 | RF-004 | Validar visualización en panel web. | Usuario autenticado en el sistema. | 1. Acceder al panel web.<br>2. Consultar estado de infraestructura. | El panel muestra estado actualizado de servidores y servicios. |
| CP-005 | RF-005 | Validar almacenamiento de métricas históricas por 24 horas. | Métricas recolectadas durante el día. | 1. Consultar métricas del día anterior. | El sistema muestra datos históricos disponibles hasta 24 horas atrás. |
| CP-006 | RF-006 | Validar instalación y configuración de agentes en servidores monitoreados. | Servidor destino disponible y con permisos de instalación. | 1. Ejecutar instalador del agente.<br>2. Configurar parámetros básicos (IP, puerto, credenciales).<br>3. Iniciar agente. | El agente queda instalado y comienza a reportar métricas al sistema. |
| CP-007 | RF-007 | Validar definición de umbrales personalizados para CPU, memoria y red. | Usuario con permisos de configuración en el sistema. | 1. Acceder a la sección de configuración.<br>2. Definir umbral CPU=80%.<br>3. Simular carga que supere el umbral. | El sistema genera alerta al superar el umbral configurado. |
| CP-008 | RF-008 | Validar exportación de reportes en formatos estándar (CSV y PDF). | Métricas disponibles en el sistema. | 1. Solicitar exportación en CSV.<br>2. Solicitar exportación en PDF.<br>3. Abrir los archivos generados. | El sistema genera archivos CSV y PDF con datos correctos y legibles. |
| CUF‑Alert‑01 | RF‑009 | Validar disparo de alerta por CPU elevada | Agente activo, umbral CPU configurado | 1. Generar carga sostenida > umbral<br>2. Consultar alertas | Se registra alerta con valor y umbral |
| CUF‑Export‑01 | RF‑010 | Validar exportación CSV correcta | Métricas disponibles en memoria | 1. Ejecutar exportación a ruta válida<br>2. Verificar archivo | Archivo CSV con encabezados y filas completas |
| CUF‑Once‑01 | RF‑011 | Validar ejecución en modo --once | Sistema estable, sin carga extraña | 1. Ejecutar `pulso --once`<br>2. Revisar salida y código | Código 0, salida JSON bien formada |
| CUF‑Health‑01 | RF‑012 | Validar endpoint de estado HTTP | Servidor HTTP activo y accesible | 1. Petición `GET /health`<br>2. Leer respuesta | Código 200, cuerpo `{"estado":"ok"}` |
| CUF-Config-01 | RF-013 | Validar consulta de configuración del agente | Servidor HTTP activo y accesible | 1. Petición `GET /config`<br>2. Leer respuesta | Código 200, JSON con `interval_ms`, `http_port` y `version` |
| CUF-Version-01 | RF-014 | Validar consulta de versión del agente | Servidor HTTP activo y accesible | 1. Petición `GET /version`<br>2. Leer respuesta | Código 200, cuerpo `{"version": "..."}` |
