# Tipos de datos del proyecto ChuspaPulse

## MetricSnapshot

`MetricSnapshot` es la estructura principal utilizada para transportar métricas del sistema entre los distintos módulos del proyecto.

### Campos

| Campo     | Tipo C++                              | Descripción                          | Unidad       |
| --------- | ------------------------------------- | ------------------------------------ | ------------ |
| cpu       | double                                | Porcentaje de uso de CPU             | %            |
| ram       | double                                | Porcentaje de uso de memoria RAM     | %            |
| disk      | double                                | Porcentaje de uso de disco           | %            |
| rx_bytes  | double                                | Bytes recibidos por la red           | bytes        |
| tx_bytes  | double                                | Bytes enviados por la red            | bytes        |
| timestamp | std::chrono::system_clock::time_point | Momento en que se capturó la métrica | fecha y hora |

### Constructores

* Constructor por defecto: inicializa todas las métricas en cero y asigna el tiempo actual a `timestamp`.
* Constructor con parámetros: permite inicializar todas las métricas al momento de crear el objeto.

### Validación

El método `isValid()` devuelve `true` únicamente cuando todas las métricas numéricas son mayores que cero.

### Ejemplo

```cpp
MetricSnapshot snapshot(
    37.5,
    62.8,
    71.3,
    1048576,
    524288
);
```

Interpretación:

* CPU: 37.5 %
* RAM: 62.8 %
* Disco: 71.3 %
* RX: 1 048 576 bytes
* TX: 524 288 bytes

---

## Configuración

### MonitorConfig

Configuración del monitor del sistema.

| Campo       | Tipo C++ | Valor por defecto | Descripción                                |
| ----------- | -------- | ----------------- | ------------------------------------------ |
| interval_ms | int      | 1000              | Intervalo de actualización en milisegundos |
| cpu         | bool     | true              | Habilita la lectura de CPU                 |
| ram         | bool     | true              | Habilita la lectura de RAM                 |
| disk        | bool     | true              | Habilita la lectura de disco               |

### ConfigServidor

| Campo  | Tipo C++    | Valor por defecto | Descripción                       |
| ------ | ----------- | ----------------- | --------------------------------- |
| host   | std::string | "0.0.0.0"         | Dirección de escucha del servidor |
| puerto | int         | 8080              | Puerto del servidor               |

### ConfigSampler

| Campo              | Tipo C++ | Valor por defecto | Descripción            |
| ------------------ | -------- | ----------------- | ---------------------- |
| intervalo_segundos | int      | 10                | Frecuencia de muestreo |

### ConfigStorage

| Campo   | Tipo C++    | Valor por defecto | Descripción              |
| ------- | ----------- | ----------------- | ------------------------ |
| ruta_db | std::string | "chuspapulse.db"        | Ruta de la base de datos |

### Config

| Campo         | Tipo C++       | Valor por defecto |
| ------------- | -------------- | ----------------- |
| servidor      | ConfigServidor | ConfigServidor()  |
| sampler       | ConfigSampler  | ConfigSampler()   |
| storage       | ConfigStorage  | ConfigStorage()   |
| nivel_log     | std::string    | "info"            |
| output_format | std::string    | "json"            |

Valores válidos para `output_format`:

* json
* csv
* prometheus

---

## Tipos de alertas

### AlertaUmbral

Genera una alerta cuando una métrica supera o cae por debajo de un valor configurado.

| Campo     | Tipo C++    | Descripción                         |
| --------- | ----------- | ----------------------------------- |
| metrica_  | std::string | Nombre de la métrica evaluada       |
| umbral_   | float       | Valor umbral de referencia          |
| operador_ | char        | Operador de comparación (`>` o `<`) |

---

## Estructuras de collectors

### RamInfo

| Campo     | Tipo C++ | Descripción               | Unidad |
| --------- | -------- | ------------------------- | ------ |
| total     | uint64_t | Memoria total del sistema | bytes  |
| used      | uint64_t | Memoria utilizada         | bytes  |
| available | uint64_t | Memoria disponible        | bytes  |

### DiskInfo

| Campo | Tipo C++ | Descripción            | Unidad |
| ----- | -------- | ---------------------- | ------ |
| total | uint64_t | Espacio total de disco | bytes  |
| used  | uint64_t | Espacio utilizado      | bytes  |
| free  | uint64_t | Espacio libre          | bytes  |

### LoadAvgInfo

| Campo  | Tipo C++ | Descripción                     | Unidad |
| ------ | -------- | ------------------------------- | ------ |
| load1  | float    | Promedio de carga de 1 minuto   | carga  |
| load5  | float    | Promedio de carga de 5 minutos  | carga  |
| load15 | float    | Promedio de carga de 15 minutos | carga  |

### ProcInfo

| Campo   | Tipo C++ | Descripción           | Unidad   |
| ------- | -------- | --------------------- | -------- |
| running | uint32_t | Procesos en ejecución | procesos |
| blocked | uint32_t | Procesos bloqueados   | procesos |
| total   | uint32_t | Total de procesos     | procesos |

### TempInfo

| Campo       | Tipo C++    | Descripción                     | Unidad   |
| ----------- | ----------- | ------------------------------- | -------- |
| cpu_celsius | float       | Temperatura del CPU             | °C       |
| zona        | std::string | Zona térmica monitoreada        | texto    |
| disponible  | bool        | Indica si existe lectura válida | booleano |

### UptimeInfo

| Campo         | Tipo C++    | Descripción                                    | Unidad   |
| ------------- | ----------- | ---------------------------------------------- | -------- |
| total_seconds | uint64_t    | Tiempo total de actividad                      | segundos |
| days          | uint32_t    | Días de actividad                              | días     |
| hours         | uint32_t    | Horas de actividad                             | horas    |
| minutes       | uint32_t    | Minutos de actividad                           | minutos  |
| seconds       | uint32_t    | Segundos de actividad                          | segundos |
| formatted     | std::string | Representación legible del tiempo de actividad | texto    |

### ContadoresCPU

| Campo     | Tipo C++ | Descripción                                    |
| --------- | -------- | ---------------------------------------------- |
| usuario   | uint64_t | Tiempo en modo usuario                         |
| nice      | uint64_t | Tiempo con prioridad ajustada                  |
| sistema   | uint64_t | Tiempo en modo kernel                          |
| ocioso    | uint64_t | Tiempo sin uso                                 |
| espera_io | uint64_t | Tiempo esperando operaciones de entrada/salida |
| irq       | uint64_t | Tiempo en interrupciones de hardware           |
| softirq   | uint64_t | Tiempo en interrupciones de software           |
| robado    | uint64_t | Tiempo utilizado por otras máquinas virtuales  |

```
```
