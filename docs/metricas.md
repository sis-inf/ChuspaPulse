# Métricas monitoreadas por ChuspaPulse

ChuspaPulse es un monitor de infraestructura ligero cuyo objetivo es proporcionar
información clave del estado del sistema en tiempo real o con baja latencia.

A continuación se describen las métricas principales que el sistema planea monitorear, junto con su definición, unidad de medida y frecuencia
esperada de muestreo.

## Tabla de métricas

| Métrica        | Descripción                                                                 | Unidad de medida        | Frecuencia de muestreo |
|----------------|-----------------------------------------------------------------------------|--------------------------|------------------------|
| CPU            | Porcentaje de uso del procesador en un intervalo de tiempo determinado.     | Porcentaje (%)           | Cada 1 segundo         |
| RAM            | Cantidad de memoria RAM utilizada y disponible en el sistema.               | Megabytes (MB) / %       | Cada 1 segundo         |
| Disco          | Uso del almacenamiento en disco, incluyendo espacio total, usado y libre.   | Gigabytes (GB) / %       | Cada 5 segundos        |
| Red            | Cantidad de datos enviados y recibidos a través de las interfaces de red.   | Bytes/segundo (B/s)      | Cada 1 segundo         |
| Procesos       | Número total de procesos activos en el sistema.                              | Cantidad (#)             | Cada 2 segundos        |
| Carga del sistema | Promedio de carga del sistema en intervalos de 1, 5 y 15 minutos.        | Valor numérico (load avg)| Cada 5 segundos        |

---

### Detalles para cada métrica

#### CPU
- **Unidad exacta**: Porcentaje (%)
- **Rango normal**: 5% - 70% en servidor estándar
- **Valor anormalmente alto (> 85%)**: Indica sobrecarga, procesos pesados o posible degradación del rendimiento
- **Valor anormalmente bajo (< 2%)**: Sistema inactivo, sin carga de trabajo significativa
- **Nombre de métrica**: `cpu.usage`

#### RAM
- **Unidad exacta**: Megabytes (MB) / Porcentaje (%)
- **Rango normal**: 20% - 75% de la memoria total
- **Valor anormalmente alto (> 90%)**: Escasez de memoria, riesgo de lentitud o uso de memoria virtual
- **Valor anormalmente bajo (< 10%)**: Recursos libres suficientes, baja ocupación
- **Nombres de métricas**: `ram.used` / `ram.total`

#### Disco
- **Unidad exacta**: Gigabytes (GB) / Porcentaje (%)
- **Rango normal**: 30% - 75% de capacidad utilizada
- **Valor anormalmente alto (> 90%)**: Espacio agotándose, riesgo de fallos al escribir datos
- **Valor anormalmente bajo (< 10%)**: Amplio espacio libre disponible
- **Nombres de métricas**: `disk.used` / `disk.total`

#### Red
- **Unidad exacta**: Bytes por segundo (B/s)
- **Rango normal**: Variable según enlace; habitual < 10 MB/s en tráfico estándar
- **Valor anormalmente alto**: Gran volumen de datos, posible saturación de red
- **Valor anormalmente bajo**: Tráfico mínimo o problemas de conectividad
- **Nombres de métricas**: `network.rx` / `network.tx`

#### Procesos
- **Unidad exacta**: Cantidad (#)
- **Rango normal**: 50 - 300 procesos activos en servidor típico
- **Valor anormalmente alto (> 500)**: Posible descontrol o fugas de procesos
- **Valor anormalmente bajo (< 20)**: Funcionamiento muy básico o con fallos
- **Campo JSON**: `process_count`

#### Carga del sistema
- **Unidad exacta**: Valor numérico (Load Average)
- **Rango normal**: Valor ≤ cantidad de núcleos del procesador
- **Valor anormalmente alto**: Valor superior a núcleos → sistema saturado
- **Valor anormalmente bajo**: Carga mínima, recursos libres
- **Campo JSON**: `load_avg_1min`, `load_avg_5min`, `load_avg_15min`

## Selección de métricas con `--metrics`

La opción `--metrics` recibe una lista de nombres separados por coma y
habilita solo las métricas indicadas.

| Nombre | Métrica |
|--------|---------|
| `cpu`  | CPU     |
| `ram`  | RAM     |
| `disk` | Disco   |

```bash
chuspapulse --metrics cpu,ram
```

Si la lista está vacía o contiene un nombre que no está en la tabla, el
programa muestra un error en la salida de errores, no modifica la selección de
métricas y termina con código distinto de cero:

```text
$ chuspapulse --metrics cpu,gpu
Error: metricas desconocidas: gpu
Metricas validas: cpu, ram, disk

$ chuspapulse --metrics ""
Error: la lista de --metrics esta vacia
Metricas validas: cpu, ram, disk
```

Los elementos vacíos dentro de la lista (por ejemplo `cpu,,ram` o `cpu,`)
también se reportan como desconocidos.

## Notas adicionales

- Las frecuencias de muestreo pueden ajustarse dependiendo del entorno (desarrollo, producción, dispositivos con recursos limitados, etc.).
- Algunas métricas pueden variar ligeramente en su implementación entre Linux y Windows debido a diferencias del sistema operativo.
- ChuspaPulse prioriza un bajo consumo de recursos, por lo que evita frecuencias excesivamente altas innecesarias.
- Las métricas pueden ampliarse en el futuro para incluir:
  - Temperatura del sistema
  - Uso de GPU
  - Latencia de red
  - Estado de servicios específicos

---

## Ejemplo de respuesta

```json
{
  "metricas": [
    {
      "nombre": "cpu.usage",
      "valor": 21.4,
      "unidad": "porcentaje",
      "timestamp": "2026-06-07T12:30:00Z"
    }
  ]
}
```