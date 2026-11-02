# Casos de Uso - Proyecto ChuspaPulse

Este documento define los actores y las funcionalidades principales del agente de monitoreo **ChuspaPulse**.

## 1. Actores del Sistema
- **Administrador de Sistemas (SysAdmin):** Responsable de configurar el agente y visualizar las métricas consolidadas.
- **Agente ChuspaPulse:** Actor automatizado que recolecta y envía datos de rendimiento en segundo plano.

## 2. Casos de Uso Principales

### CU-01: Registro de Métricas de CPU
- **Actor:** Agente ChuspaPulse
- **Descripción:** El agente captura el porcentaje de uso de la CPU en intervalos definidos.
- **Flujo Principal:**
  1. El agente inicia el ciclo de captura.
  2. Consulta el estado del procesador mediante llamadas al sistema.
  3. Almacena temporalmente el valor capturado para su envío.

### CU-02: Consulta del Estado General del Sistema
- **Actor:** Administrador de Sistemas
- **Descripción:** Permite al administrador obtener un resumen completo del estado actual del sistema desde la terminal.
- **Flujo Principal:**
  1. El administrador ejecuta `./pulso --status` desde la terminal.
  2. El Agente ChuspaPulse consulta las métricas actuales al sistema operativo.
  3. Se muestra el estado general: procesos activos, uso de recursos y estado de la red.

### CU-03: Visualización de Estado de Red
- **Actor:** Administrador de Sistemas
- **Descripción:** Permite ver la velocidad de descarga, subida y el estado de la conexión de red del sistema.
- **Flujo Principal:**
  1. El administrador solicita ver el estado del sistema vía menú o `--status`.
  2. El Agente ChuspaPulse consulta las métricas de red al sistema operativo.
  3. Se muestra la velocidad de descarga, subida en Mbps y el estado de conexión.

### CU-04: Navegación por el Menú Interactivo
- **Actor:** Administrador de Sistemas
- **Descripción:** Permite al administrador interactuar con ChuspaPulse a través de un menú de opciones en la terminal.
- **Flujo Principal:**
  1. El administrador ejecuta `./pulso` sin argumentos.
  2. ChuspaPulse presenta el menú con las opciones: Ver estado, Actualizar datos, Salir.
  3. El administrador selecciona una opción ingresando el número correspondiente.
  4. ChuspaPulse ejecuta la acción y muestra el resultado.

### CU-05: Recolección de Métricas de Memoria RAM
- **Actor:** Agente ChuspaPulse
- **Descripción:** El agente captura el estado actual de la memoria RAM del sistema, incluyendo uso total y disponible.
- **Flujo Principal:**
  1. El agente consulta la memoria RAM al sistema operativo.
  2. Obtiene la memoria total, en uso y disponible.
  3. Almacena temporalmente los valores para mostrarlos al administrador.

---

### CU-Alert-01: Configuración de alerta por uso elevado de CPU
- **Actor:** Administrador de Sistemas
- **Descripción:** Permite definir un umbral de uso de CPU para generar notificaciones automáticas cuando se supere el límite establecido.
- **Flujo Principal:**
  1. El administrador abre el archivo de configuración `config.toml`.
  2. Localiza la sección `[alertas.cpu]`.
  3. Establece el valor `max = 90` (90% de uso máximo).
  4. Configura el intervalo de revisión: `intervalo_revision = 60` segundos.
  5. Guarda el archivo de configuración.
  6. Reinicia el servicio para aplicar los cambios:
     ```bash
     sudo systemctl restart pulso
     ```
- **Postcondición:** El sistema generará una alerta cuando el uso de CPU supere el 90% durante 3 mediciones consecutivas.

---

### CU-Export-01: Exportación de historial de métricas a CSV
- **Actor:** Administrador de Sistemas
- **Descripción:** Permite extraer el historial de métricas almacenado y guardarlo en formato CSV para su análisis en herramientas externas.
- **Flujo Principal:**
  1. El administrador abre la terminal en el directorio raíz del proyecto.
  2. Ejecuta el comando de exportación:
     ```bash
     ./build/bin/pulso --export csv --output metricas.csv --last-hours 24
     ```
  3. El sistema procesa los datos y genera el archivo de salida con el historial completo.
- **Postcondición:** Se crea el archivo `metricas.csv` con los valores de CPU, memoria, disco y red de las últimas 24 horas.

---

### CU-Once-01: Recolección puntual de métricas
- **Actor:** Administrador de Sistemas
- **Descripción:** Ejecuta una sola captura de métricas del sistema sin dejar el servicio ejecutándose en segundo plano.
- **Flujo Principal:**
  1. El administrador abre la terminal en el directorio raíz del proyecto.
  2. Ejecuta el modo de captura única:
     ```bash
     ./build/bin/pulso --once
     ```
  3. El agente obtiene los valores actuales de todos los recursos y los muestra en pantalla.
- **Postcondición:** Se muestran las métricas del momento y el proceso finaliza automáticamente.
