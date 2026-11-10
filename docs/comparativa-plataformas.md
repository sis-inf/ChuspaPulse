# Comparativa de Métricas entre Linux y Windows

## Descripción

ChuspaPulse soporta Linux y Windows, pero no todas las métricas son equivalentes entre plataformas. Este documento detalla las diferencias para evitar conclusiones erróneas al comparar valores entre hosts de distinto sistema operativo.

---

##  Advertencia: loadavg no es equivalente entre plataformas

> **loadavg en Windows es una aproximación, no un equivalente exacto al de Linux.**
>
> En Linux, el load average es un valor nativo del kernel que refleja la cantidad de procesos en estado de ejecución o espera de I/O en los últimos 1, 5 y 15 minutos.
>
> En Windows, no existe un concepto nativo equivalente. ChuspaPulse calcula una **aproximación** basada en el uso de CPU y la cola de procesos, pero el valor **no es directamente comparable** con el load average de Linux.
>
> **No compares valores de loadavg entre un host Linux y uno Windows.**

---

## Tabla comparativa de métricas

| Métrica | Campo JSON | Linux | Windows | Equivalencia |
|---------|-----------|-------|---------|--------------|
| CPU | `cpu_percent` | Uso total del procesador (%) | Uso total del procesador (%) |  Equivalente |
| RAM usada | `ram_used_mb` | Memoria usada en MB | Memoria usada en MB |  Equivalente |
| RAM usada (%) | `ram_used_percent` | % sobre RAM total | % sobre RAM total |  Equivalente |
| Disco usado | `disk_used_gb` | Espacio usado en GB | Espacio usado en GB |  Equivalente |
| Disco usado (%) | `disk_used_percent` | % sobre capacidad total | % sobre capacidad total |  Equivalente |
| Red recibida | `net_rx_bps` | Bytes/s recibidos | Bytes/s recibidos |  Equivalente |
| Red enviada | `net_tx_bps` | Bytes/s enviados | Bytes/s enviados |  Equivalente |
| Procesos | `process_count` | Procesos activos del kernel | Procesos activos |  Equivalente |
| Load avg 1 min | `load_avg_1min` | Nativo del kernel | Aproximación basada en CPU |  Aproximación |
| Load avg 5 min | `load_avg_5min` | Nativo del kernel | Aproximación basada en CPU |  Aproximación |
| Load avg 15 min | `load_avg_15min` | Nativo del kernel | Aproximación basada en CPU |  Aproximación |
| Batería | `battery_percent` | Disponible si hay batería | Disponible si hay batería |  Equivalente |

### Leyenda

| Estado | Significado |
|---------|-------------|
|  Equivalente | El valor tiene el mismo significado en ambas plataformas y es comparable directamente. |
|  Aproximación | El valor existe en ambas plataformas pero **no es directamente comparable**. |
|  Exclusivo | La métrica solo está disponible en una plataforma. |

---

## Detalles por métrica

### CPU
Ambas plataformas reportan el porcentaje de uso del procesador de forma equivalente. Los valores son comparables directamente.

### RAM y Disco
El cálculo de memoria y disco es equivalente entre plataformas. Los valores en MB, GB y porcentaje son comparables.

### Red
Los bytes enviados y recibidos se calculan de forma equivalente en ambas plataformas.

### Procesos
El conteo de procesos activos es comparable, aunque en Windows puede incluir procesos del sistema que no tienen equivalente en Linux.

### Load Average 
- **Linux:** valor nativo proporcionado por el kernel en `/proc/loadavg`. Refleja la cantidad de procesos en estado runnable o esperando I/O.
- **Windows:** no existe un equivalente nativo. ChuspaPulse calcula una aproximación basada en métricas de rendimiento del sistema.
- **Conclusión:** no uses loadavg para comparar carga entre hosts Linux y Windows.

---

## Notas

- Las métricas marcadas como (equivalente) son seguras para comparar entre plataformas.
- Para métricas (aproximacion), usa cada valor solo como referencia dentro de su propia plataforma.
- Esta tabla se actualizará a medida que se agreguen nuevas métricas al proyecto.