# Arquitectura del Sistema ChuspaPulse

## Módulos principales

| Módulo | Responsabilidad |
|--------|----------------|
| **Collector** | Recolecta métricas del sistema (CPU, RAM, disco, red, procesos, temperatura y uptime) desde `/proc` y `/sys` |
| **Storage** | Almacena las métricas en memoria o disco para análisis histórico |
| **Analyzer** | Procesa las métricas y detecta anomalías o tendencias |
| **API** | Expone las métricas vía HTTP para consultas externas |
| **Alert Manager** | Gestiona alertas mediante AlertaUmbral y GestorAlertas |
| **Logger** | Registra eventos, errores y actividad del sistema |

## 1. Introducción
La arquitectura de software de **ChuspaPulse** define la organización estructural del sistema, sus responsabilidades y las interconexiones entre sus componentes. Este diseño responde a la necesidad de monitorear recursos críticos de hardware (CPU, RAM, Red) con un enfoque en la eficiencia, escalabilidad y disponibilidad de datos en tiempo real.

## Diagrama de componentes (Mermaid)

```mermaid
graph TD
    A[Sistema ChuspaPulse] --> B[Collector]
    A --> C[Storage]
    A --> D[Analyzer]
    A --> E[API]
    A --> F[Alert Manager]
    A --> G[Logger]

    B --> H["/proc/stat, /proc/meminfo"]
    C --> I["Métricas históricas"]
    D --> J["Detección de anomalías"]
    E --> K[Cliente HTTP]
    F --> L[Notificaciones]
    G --> M[pulso.log]
```

## 2. Visión General del Sistema
ChuspaPulse es una plataforma de monitoreo distribuida. A diferencia de herramientas CLI aisladas, ChuspaPulse integra un modelo de recolección de datos mediante agentes que se comunican con una infraestructura central para la persistencia, alerta y visualización mediante una interfaz web moderna.

---

## 3. Componentes Principales
El sistema se divide en módulos fundamentales, cada uno cumpliendo una función específica dentro del ciclo de vida del dato:

* **Agente Recolector (C++):** Módulo de bajo nivel que interactúa directamente con las APIs del Kernel (ej. `/proc` en Linux, `WinAPI` en Windows). Extrae métricas precisas de:
    * **CPU:** Carga por núcleo, frecuencia y tiempo de interrupción.
    * **RAM:** Memoria total, disponible, swap y buffers.
    * **Red:** Paquetes transmitidos/recibidos, errores de interfaz y ancho de banda.
* **Almacenamiento de Métricas:** Estructura de persistencia encargada de mantener el historial de las métricas recolectadas para análisis temporal y comparativas.
* **Módulo de Alertas:** Motor de reglas que evalúa las métricas en tiempo real. Dispara notificaciones cuando se superan umbrales críticos (ej. CPU > 90%).
* **Panel Web (Frontend):** Interfaz gráfica de usuario que permite la visualización intuitiva mediante gráficas dinámicas y dashboards de estado.
### Subsistema de Alertas

El sistema incorpora un subsistema de alertas encargado de detectar condiciones críticas en las métricas recolectadas.

#### Componentes

- **AlertaUmbral:** Evalúa métricas individuales y determina si superan los umbrales configurados.
- **GestorAlertas:** Administra las alertas generadas y coordina su consulta mediante la API.

#### Flujo de Alertas

1. Los collectors generan métricas periódicamente.
2. Las métricas son procesadas por el sistema.
3. AlertaUmbral verifica si existen valores fuera de los límites establecidos.
4. GestorAlertas registra y expone las alertas activas.

## HTTP Handlers

La API HTTP del sistema expone distintos handlers encargados de responder solicitudes de monitoreo y administración.

| Handler | Responsabilidad |
|----------|----------------|
| **handler_health** | Proporciona información sobre el estado operativo del sistema y permite realizar verificaciones de salud (*health checks*). |
| **handler_config** | Gestiona consultas relacionadas con la configuración del sistema. |
| **handler_alerts** | Permite consultar y exponer las alertas generadas por el subsistema de alertas. |

Estos handlers actúan como punto de entrada para clientes HTTP y facilitan la integración con herramientas externas de monitoreo y observabilidad.

### Nuevos Collectors

Además de las métricas tradicionales de CPU, memoria y red, ChuspaPulse incorpora nuevos collectors especializados:

| Collector | Descripción |
|------------|------------|
| **Temperature Collector** | Obtiene la temperatura del sistema desde sensores disponibles. |
| **Uptime Collector** | Reporta el tiempo de actividad continuo del sistema. |
| **Load Average Collector** | Obtiene el promedio de carga del sistema para diferentes intervalos de tiempo. |
| **Process Collector** | Obtiene estadísticas de procesos del sistema, incluyendo procesos en ejecución, bloqueados y total de procesos mediante `/proc/stat` y `/proc/loadavg`. |
---

## 4. Tecnologías Utilizadas
Se han seleccionado herramientas que garantizan la interoperabilidad y el rendimiento exigido en los criterios de aceptación.

| Componente | Tecnología | Versión | Justificación |
| :--- | :--- | :--- | :--- |
| **Núcleo / Agente** | C++ | C++17 | Control granular de memoria y acceso directo a recursos del sistema. |
| **Compilador** | GCC / Clang | Reciente | Optimización de binarios y cumplimiento de estándares modernos. |
| **Build System** | CMake | 3.10+ | Estándar de la industria para proyectos multiplataforma en C++. |
| **Visualización** | Panel Web | HTML/JS | Facilita el acceso remoto a las métricas de forma visual y moderna. |
| **Librerías Ext.** | Nlohmann/JSON | - | Manejo eficiente de transferencia de datos entre el agente y el panel. |

---

## 5. Diagrama de Arquitectura
El siguiente diagrama describe la interacción entre los componentes definidos para el sistema ChuspaPulse.

```mermaid
graph TD
    subgraph Nivel_Sistema_Host
        A[Agente Recolector] -->|Sonda de Hardware| B[CPU / RAM / Red]
    end

    subgraph Servicios_Centrales
        A -->|Envio de Datos JSON| C[Almacenamiento de Metricas]
        C --> D[Modulo de Alertas]
        D -->|Trigger| E[Umbral excedido]
        E -->|Si| F[Notificacion / Alerta]
    end

    subgraph Capa_Presentacion
        C -->|Fetch Data| G[Panel Web]
        G -->|Visualizacion| H[Usuario Final]
    end
    D --> H[AlertaUmbral]
    H --> I[GestorAlertas]

    A --> J[Temperature Collector]
    A --> K[Uptime Collector]
    A --> L[Load Average Collector]

    G --> M[handler_health]
    G --> N[handler_config]
    G --> O[handler_alerts]
    style A fill:#f96,stroke:#333,stroke-width:2px
    style G fill:#6cf,stroke:#333,stroke-width:2px
```

---

## 6. Decisiones Técnicas (ADR)
### 6.1 Elección de C++17 para el Agente
* **Contexto:** Se requiere un monitoreo constante y preciso de los recursos de hardware sin impactar negativamente en el rendimiento del servidor o host donde se ejecuta la herramienta.
* **Decisión:** Se optó por el uso de C++17 en lugar de lenguajes interpretados o de alto nivel como Python.
* **Consecuencia:** El agente logra mantener una huella de memoria mínima y un uso de CPU insignificante, cumpliendo con los estándares de eficiencia técnica para herramientas de monitoreo profesional y sistemas de misión crítica.

### 6.2 Implementación de Panel Web sobre CLI
* **Contexto:** Las métricas generadas por el sistema deben ser consultadas de forma visual, intuitiva y centralizada por el equipo técnico o los administradores de sistemas.
* **Decisión:** Desarrollo de un panel web dedicado para la visualización de datos, complementando las capacidades básicas de la terminal.
* **Consecuencia:** Se mejora significativamente la accesibilidad remota y se permite la representación de tendencias temporales mediante gráficas de alta fidelidad, facilitando la toma de decisiones basada en datos históricos.

---

## 7. Flujo de Datos
El recorrido de una métrica, desde su recolección en el hardware hasta su visualización final en la interfaz de usuario, sigue este flujo lógico y secuencial:

1. **Recolección:** El **Agente Recolector** obtiene el estado actual del hardware (CPU, RAM, Red) realizando consultas directas a las APIs del sistema operativo.
2. **Estructuración:** Los datos brutos recolectados se transforman a formato **JSON** para asegurar la interoperabilidad y el intercambio de información entre el Agente (C++) y el Panel.
3. **Persistencia:** El componente de **Almacenamiento** recibe la métrica estructurada y la guarda en la base de datos con su respectiva marca de tiempo (*timestamp*).
4. **Análisis:** El **Módulo de Alertas** evalúa el dato en tiempo real; si el valor detectado supera los umbrales de seguridad predefinidos, dispara una notificación de advertencia.
5. **Visualización:** El **Panel Web** consulta los datos almacenados y renderiza las métricas en un formato gráfico e interactivo para el usuario final.