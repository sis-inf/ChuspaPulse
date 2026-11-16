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
- **Descripción:** Permite al administrador obtener una lectura puntual de las métricas del sistema (CPU, memoria, disco, red, entre otras) desde la terminal.
- **Flujo Principal:**
  1. El administrador ejecuta desde la terminal:
     ```bash
     ./build/bin/chuspapulse --once --metrics cpu,ram,disk
     ```
  2. El Agente ChuspaPulse consulta las métricas actuales al sistema operativo.
  3. Se muestran los valores recolectados y el proceso finaliza.

### CU-03: Visualización de Estado de Red
- **Actor:** Administrador de Sistemas
- **Descripción:** Permite consultar las métricas de red del sistema recolectadas por ChuspaPulse.
- **Flujo Principal:**
  1. El administrador ejecuta la captura puntual:
     ```bash
     ./build/bin/chuspapulse --once
     ```
  2. El Agente ChuspaPulse consulta las métricas de red al sistema operativo.
  3. Se muestran las métricas de red junto con el resto de la captura.

### CU-04: Ejecución Continua con Intervalo y Métricas Seleccionadas
- **Actor:** Administrador de Sistemas
- **Descripción:** Permite al administrador ejecutar ChuspaPulse en la terminal de forma continua, eligiendo el intervalo de muestreo y las métricas a recolectar.
- **Flujo Principal:**
  1. El administrador ejecuta:
     ```bash
     ./build/bin/chuspapulse --interval 5000 --metrics cpu,ram
     ```
  2. ChuspaPulse recolecta las métricas indicadas (CPU y RAM) cada 5000 ms.
  3. El administrador detiene la ejecución con `Ctrl+C` cuando lo desee.

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
     sudo systemctl restart chuspapulse
     ```
- **Postcondición:** El sistema generará una alerta cuando el uso de CPU supere el 90% durante 3 mediciones consecutivas.

---

### CU-Export-01: Exportación de una captura de métricas a CSV
- **Actor:** Administrador de Sistemas
- **Descripción:** Permite guardar una captura puntual de métricas en formato CSV para su análisis en herramientas externas. La exportación de historial por rango de horas todavía no está disponible.
- **Flujo Principal:**
  1. El administrador abre la terminal en el directorio raíz del proyecto.
  2. Ejecuta la captura única con formato CSV y redirige la salida a un archivo:
     ```bash
     ./build/bin/chuspapulse --once --format csv > metricas.csv
     ```
  3. El agente obtiene los valores actuales y los escribe en el archivo.
- **Postcondición:** Se crea el archivo `metricas.csv` con los valores de CPU, memoria y disco del momento de la captura.

---

### CU-Once-01: Recolección puntual de métricas
- **Actor:** Administrador de Sistemas
- **Descripción:** Ejecuta una sola captura de métricas del sistema sin dejar el servicio ejecutándose en segundo plano.
- **Flujo Principal:**
  1. El administrador abre la terminal en el directorio raíz del proyecto.
  2. Ejecuta el modo de captura única:
     ```bash
     ./build/bin/chuspapulse --once
     ```
  3. El agente obtiene los valores actuales de las métricas habilitadas y los muestra en pantalla.
- **Postcondición:** Se muestran las métricas del momento y el proceso finaliza automáticamente.
