# Diseño inicial de clases C++ — Sistema ChuspaPulse

Este documento describe las clases principales que compondran el sistema ChuspaPulse.
Cada clase se presenta con su responsabilidad, atributos principales y metodos principales.

---

## Clase `SystemMonitor`

| Elemento | Detalle |
|---|---|
| **Responsabilidad** | Coordinar la recoleccion y presentacion del estado general del sistema. Es el punto de entrada principal de la aplicacion. |
| **Atributos principales** | `cpuInfo: CpuInfo`, `memoryInfo: MemoryInfo`, `diskInfo: DiskInfo`, `networkInfo: NetworkInfo` |
| **Metodos principales** | `run()`, `showStatus()`, `showWelcome()`, `handleOption(int option)` |

**Descripcion:**
`SystemMonitor` actua como controlador central. Inicializa los modulos de informacion, gestiona el menu interactivo y delega a cada modulo especializado la recoleccion de sus datos. Corresponde a la salida del comando `./ChuspaPulse` y `./ChuspaPulse --status`.

---

## Clase `CpuInfo`

| Elemento | Detalle |
|---|---|
| **Responsabilidad** | Recopilar y exponer metricas de uso de la CPU del sistema. |
| **Atributos principales** | `usagePercent: float`, `activeCores: int`, `totalCores: int` |
| **Metodos principales** | `collect()`, `getUsagePercent()`, `getActiveCores()`, `display()` |

**Descripción:**
`CpuInfo` encapsula toda la logica relacionada con el procesador. El metodo `collect()` consulta el sistema operativo para actualizar los atributos. `display()` formatea la salida para presentarla al usuario, por ejemplo:

```
CPU:
- Uso: 23%
- Nucleos activos: 4
```

---

## Clase `MemoryInfo`

| Elemento | Detalle |
|---|---|
| **Responsabilidad** | Recopilar y exponer metricas de uso de la memoria RAM del sistema. |
| **Atributos principales** | `usedGB: float`, `totalGB: float`, `availableGB: float` |
| **Metodos principales** | `collect()`, `getUsedGB()`, `getAvailableGB()`, `display()` |

**Descripcion:**
`MemoryInfo` consulta la memoria disponible y en uso. Permite conocer en todo momento cuantos recursos de RAM estan siendo consumidos, apoyando la salida esperada:

```
Memoria (RAM):
- Uso: 3.2 GB / 8 GB
- Disponible: 4.8 GB
```

---

## Clase `DiskInfo`

| Elemento | Detalle |
|---|---|
| **Responsabilidad** | Recopilar y exponer metricas de uso del almacenamiento en disco. |
| **Atributos principales** | `usedGB: float`, `totalGB: float`, `freeGB: float` |
| **Metodos principales** | `collect()`, `getUsedGB()`, `getFreeGB()`, `display()` |

**Descripcion:**
`DiskInfo` obtiene informacion del sistema de archivos para reportar el espacio utilizado y disponible en disco. Su salida esperada es:

```
Disco:
- Uso: 120 GB / 256 GB
- Espacio libre: 136 GB
```

---

## Clase `NetworkInfo`

| Elemento | Detalle |
|---|---|
| **Responsabilidad** | Recopilar y exponer metricas de conectividad y uso de red. |
| **Atributos principales** | `downloadMbps: float`, `uploadMbps: float`, `status: std::string` |
| **Metodos principales** | `collect()`, `getDownloadSpeed()`, `getUploadSpeed()`, `isConnected()`, `display()` |

**Descripcion:**
`NetworkInfo` mide el ancho de banda de descarga y subida, y verifica el estado de la conexion. Su salida esperada es:

```
Red:
- Descarga: 12 Mbps
- Subida: 3 Mbps
- Estado: Conectado
```

---


---

## Clase GestorAlertas

| Elemento            | Detalle                                                                 |
|---------------------|-------------------------------------------------------------------------|
| Responsabilidad     | Administra la configuración, evaluación y emisión de alertas; implementa `IGestorAlertas`. |
| Atributos principales | umbrales: lista de `AlertaUmbral`, colecciones: lista de `ICollector` |
| Metodos principales | evaluar(), registrarUmbral(), eliminarUmbral(), notificar()             |

**Descripción**: Recibe datos de los recolectores, compara con umbrales definidos y dispara alertas cuando se cumplen las condiciones.

---

## Clase AlertaUmbral

| Elemento            | Detalle                                                                 |
|---------------------|-------------------------------------------------------------------------|
| Responsabilidad     | Define regla de activación por valor límite; implementa `IAlerta`.     |
| Atributos principales | tipoMetrica: string, valorLimite: float, esSuperior: bool, mensaje: string |
| Metodos principales | evaluar(valor: float): bool, obtenerMensaje(): string                   |

**Descripción**: Alerta concreta que se activa al superar o caer por debajo de un umbral configurado.

---

## Manejadores HTTP nuevos

### HandlerMetricas
- **Responsabilidad**: Endpoint `GET /metrics` para exponer métricas en formato Prometheus.
- **Interfaz**: `IHandlerHTTP`
- **Métodos**: `handle(Request, Response)`

### HandlerHealth
- **Responsabilidad**: Endpoint `GET /health` para reportar estado general del servicio.
- **Interfaz**: `IHandlerHTTP`
- **Métodos**: `handle(Request, Response)`

### HandlerAlertas
- **Responsabilidad**: Consultar alertas activas e historial.
- **Interfaz**: `IHandlerHTTP`
- **Métodos**: `handle(Request, Response)`

---

## Recolectores adicionales

### CollectorTemperatura
- **Responsabilidad**: Leer sensores térmicos del sistema.
- **Interfaz**: `ICollector`
- **Métodos**: `collect()`

### CollectorUptime
- **Responsabilidad**: Obtener tiempo de actividad desde `/proc/uptime`.
- **Interfaz**: `ICollector`
- **Métodos**: `collect()`

### CollectorLoadAvg
- **Responsabilidad**: Obtener promedio de carga desde `/proc/loadavg`.
- **Interfaz**: `ICollector`
- **Métodos**: `collect()`

### CollectorProcesos
- **Responsabilidad**: Listar y resumir procesos activos.
- **Interfaz**: `ICollector`
- **Métodos**: `collect()`


## Resumen de clases

| Clase | Responsabilidad principal |
|---|---|
| `SystemMonitor` | Controlador central: menu, flujo de la app y coordinacion de modulos |
| `CpuInfo` | Metricas del procesador (uso y nucleos activos) |
| `MemoryInfo` | Metricas de memoria RAM (uso y disponibilidad) |
| `DiskInfo` | Metricas de almacenamiento en disco (uso y espacio libre) |
| `NetworkInfo` | Metricas de red (velocidad y estado de conexion) |
| `GestorAlertas`      | Coordinar evaluación y disparo de alertas |
| `AlertaUmbral`        | Definir reglas de alerta basadas en umbrales |
| `HandlerMetricas`     | Exponer métricas por HTTP en formato Prometheus |
| `HandlerHealth`      | Responder estado de salud del servicio |
| `HandlerAlertas`      | Consultar alertas activas e historial |
| `CollectorTemperatura`| Recolectar temperatura de sensores |
| `CollectorUptime`     | Recolectar tiempo de actividad |
| `CollectorLoadAvg`    | Recolectar promedio de carga del sistema |
| `CollectorProcesos`   | Recolectar lista y estado de procesos |

---

## Relaciones entre módulos
- Todos los recolectores implementan **`ICollector`** y son usados por **`SystemMonitor`** y **`GestorAlertas`**.
- **`GestorAlertas`** trabaja con objetos **`AlertaUmbral`**.
- Los manejadores HTTP leen datos del núcleo y pueden consultar el estado de **`GestorAlertas`**.

