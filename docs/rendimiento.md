# Rendimiento y Requisitos de Hardware

## Consumo de recursos del agente

ChuspaPulse está diseñado para ser un agente de monitoreo ligero con un consumo de recursos mínimo. Al estar implementado en C++, el agente tiene una huella de memoria pequeña y un uso de CPU bajo, lo que lo hace ideal para servidores con recursos limitados o entornos donde cada MB de RAM cuenta.

### Consumo de memoria

- **Memoria base (RSS)**: ~15-25 MB cuando el agente está inactivo
- **Memoria durante muestreo**: ~20-30 MB durante la recolección de métricas
- **Memoria máxima**: ~40-50 MB en condiciones de carga alta con historial extenso

El consumo de memoria se mantiene estable a lo largo del tiempo gracias a que el agente no acumula datos en memoria indefinidamente. Los snapshots se escriben en la base de datos SQLite inmediatamente después de cada ciclo de muestreo.

### Consumo de CPU

- **CPU en reposo**: <0.1% de un núcleo (principalmente espera activa en el intervalo de muestreo)
- **CPU durante muestreo**: 1-3% de un núcleo durante 50-100ms por ciclo
- **CPU promedio**: <0.5% de un núcleo con intervalo de muestreo de 10 segundos

El uso de CPU es proporcional al número de collectors activos y a la complejidad de las operaciones de I/O requeridas para leer las fuentes de datos del kernel.

### Consumo de disco

- **Base de datos SQLite**: ~1-2 MB por día de operación con intervalo de 10 segundos
- **Logs**: <1 MB por día con nivel de log `info`
- **Binario compilado**: ~2-3 MB (dependiendo de las optimizaciones de compilación)

El crecimiento de la base de datos es lineal y predecible. Se puede configurar retención de datos para limitar el crecimiento a largo plazo.

### Consumo de red

- **Tráfico HTTP**: ~1-2 KB por solicitud a `/metrics` o `/health`
- **Tráfico `/metrics/history`**: Variable según el parámetro `limite` (aprox. 500 bytes por snapshot)
- **Prometheus scraping**: ~2-3 KB por cada scrape de 15 segundos

El agente no realiza conexiones salientes; todo el tráfico es iniciado por clientes externos.

## Requisitos mínimos de hardware

ChuspaPulse puede ejecutarse en hardware muy modesto gracias a su diseño eficiente. Los siguientes son los requisitos mínimos recomendados para una operación estable:

### Requisitos mínimos

- **CPU**: 1 núcleo a 500 MHz (cualquier arquitectura x86_64 o ARM)
- **RAM**: 64 MB disponibles (después del sistema operativo)
- **Disco**: 100 MB libres para binario, base de datos y logs
- **Sistema operativo**: Linux kernel 3.10 o superior (con /proc y /sys montados)

### Requisitos recomendados

- **CPU**: 1 núcleo a 1 GHz o superior
- **RAM**: 128 MB disponibles
- **Disco**: 500 MB libres para permitir crecimiento histórico
- **Sistema operativo**: Linux kernel 4.0 o superior

### Requisitos para entornos de producción

- **CPU**: 2 núcleos a 1 GHz o superior
- **RAM**: 256 MB disponibles
- **Disco**: 1 GB libres con partición separada para datos
- **Sistema operativo**: Linux kernel 5.0 o superior con soporte SQLite3

### Dependencias del sistema

- **libsqlite3-dev**: Para almacenamiento de métricas históricas
- **libcurl4-openssl-dev**: Para servidor HTTP (opcional, se puede usar implementación alternativa)
- **C++17**: Compilador compatible (g++ 7+, clang++ 5+)
- **CMake 3.16+**: Sistema de build

## Impacto por intervalo de muestreo

El intervalo de muestreo configurado en `ChuspaPulse.toml` (parámetro `intervalo_segundos` en la sección `[sampler]`) tiene un impacto directo en el consumo de recursos y la resolución de las métricas.

### Intervalos cortos (1-5 segundos)

- **Ventajas**: Alta resolución temporal, detección rápida de anomalías
- **Desventajas**: Mayor consumo de CPU, crecimiento más rápido de la base de datos
- **Consumo de CPU**: 2-5% promedio de un núcleo
- **Crecimiento de DB**: ~5-10 MB por día
- **Uso recomendado**: Entornos de desarrollo o sistemas críticos que requieren monitoreo granular

### Intervalos medios (10-30 segundos) - **Recomendado**

- **Ventajas**: Balance entre resolución y consumo de recursos
- **Desventajas**: Puede perder picos muy cortos (<10 segundos)
- **Consumo de CPU**: 0.3-1% promedio de un núcleo
- **Crecimiento de DB**: ~1-3 MB por día
- **Uso recomendado**: Producción general, servidores estándar

### Intervalos largos (60-300 segundos)

- **Ventajas**: Mínimo consumo de recursos, crecimiento lento de DB
- **Desventajas**: Baja resolución, no adecuado para alertas en tiempo real
- **Consumo de CPU**: <0.2% promedio de un núcleo
- **Crecimiento de DB**: ~0.2-0.5 MB por día
- **Uso recomendado**: Servidores con recursos muy limitados, monitoreo de tendencias a largo plazo

### Impacto en la base de datos

El tamaño de la base de datos SQLite crece linealmente con el número de snapshots almacenados. Con la configuración por defecto (intervalo de 10 segundos):

- **Snapshots por día**: 8,640
- **Tamaño por snapshot**: ~200-300 bytes
- **Crecimiento diario**: ~1.7-2.6 MB
- **Crecimiento mensual**: ~50-75 MB

Se puede implementar una política de retención (por ejemplo, eliminar snapshots más antiguos de 30 días) para limitar el crecimiento.

## Cómo medir el rendimiento del agente

Para evaluar el rendimiento de ChuspaPulse en un entorno específico, se recomienda seguir este procedimiento:

### Herramientas de medición

```bash
# Medir consumo de memoria y CPU en tiempo real
pidstat -p $(pgrep ChuspaPulse) 1

# Medir consumo de memoria detallado
ps aux | grep ChuspaPulse

# Medir latencia de endpoints
curl -w "@curl-format.txt" -o /dev/null -s http://localhost:8080/metrics

# Medir rendimiento de la base de datos
sqlite3 ChuspaPulse.db "PRAGMA cache_size;"
sqlite3 ChuspaPulse.db "SELECT COUNT(*) FROM snapshots;"
```

### Script de benchmark

```bash
#!/bin/bash
# benchmark_ChuspaPulse.sh

echo "=== Benchmark de ChuspaPulse ==="
echo "Iniciando agente en background..."
./build/bin/ChuspaPulse --config ChuspaPulse.toml &
CHUSPAPULSE_PID=$!

sleep 5

echo "=== Métricas iniciales ==="
ps -p $CHUSPAPULSE_PID -o pid,vsz,rss,pcpu,pmem,cmd

echo "=== Prueba de carga: 100 solicitudes a /metrics ==="
for i in {1..100}; do
    curl -s http://localhost:8080/metrics > /dev/null
done

echo "=== Métricas después de carga ==="
ps -p $CHUSPAPULSE_PID -o pid,vsz,rss,pcpu,pmem,cmd

echo "=== Latencia promedio de /metrics ==="
time for i in {1..10}; do
    curl -s http://localhost:8080/metrics > /dev/null
done

echo "=== Tamaño de base de datos ==="
du -h ChuspaPulse.db

echo "=== Deteniendo agente ==="
kill $CHUSPAPULSE_PID
```

### Métricas clave a monitorear

- **RSS (Resident Set Size)**: Memoria física utilizada
- **VSZ (Virtual Memory Size)**: Memoria virtual total asignada
- **%CPU**: Porcentaje de uso de CPU
- **Latencia de endpoints**: Tiempo de respuesta de HTTP
- **Tamaño de DB**: Crecimiento de la base de datos SQLite
- **I/O de disco**: Operaciones de lectura/escritura

### Comparación antes/después

Para evaluar el impacto de cambios en el código o configuración:

1. Establecer una línea base con la versión actual
2. Ejecutar el script de benchmark y registrar los resultados
3. Aplicar los cambios (nueva versión, configuración modificada)
4. Repetir el benchmark en las mismas condiciones
5. Comparar los resultados para identificar regresiones o mejoras

## Comparación con otros agentes

ChuspaPulse ofrece ventajas significativas frente a otras soluciones de monitoreo en términos de consumo de recursos y simplicidad.

### Comparación con Node Exporter (Prometheus)

| Métrica | ChuspaPulse | Node Exporter |
|---------|-------|---------------|
| Memoria base | 15-25 MB | 30-50 MB |
| CPU en reposo | <0.1% | 0.2-0.5% |
| Binario | 2-3 MB | 8-12 MB |
| Dependencias | SQLite3, libcurl | Ninguna (estático) |
| Almacenamiento histórico | SQLite integrado | No (requiere Prometheus) |
| Endpoints HTTP | 4 (health, metrics, history, prometheus) | ~20 (más especializados) |
| Configuración | 1 archivo TOML | Flags de línea de comandos |

**Ventajas de ChuspaPulse**: Almacenamiento histórico integrado, configuración más simple, menor consumo de memoria.
**Ventajas de Node Exporter**: Sin dependencias externas, más collectors especializados, ecosistema más amplio.

### Comparación con Telegraf (InfluxData)

| Métrica | ChuspaPulse | Telegraf |
|---------|-------|----------|
| Memoria base | 15-25 MB | 40-80 MB |
| CPU en reposo | <0.1% | 0.5-1% |
| Binario | 2-3 MB | 15-25 MB |
| Dependencias | SQLite3, libcurl | Múltiples plugins |
| Lenguaje | C++ | Go |
| Almacenamiento | SQLite integrado | Requiere base de datos externa |
| Plugins | 7 collectors básicos | 200+ plugins |
| Configuración | TOML simple | TOML complejo con muchos plugins |

**Ventajas de ChuspaPulse**: Mucho menor consumo de recursos, más simple de configurar y mantener.
**Ventajas de Telegraf**: Ecosistema de plugins enorme, soporte para múltiples protocolos de salida.

### Comparación con Collectd

| Métrica | ChuspaPulse | Collectd |
|---------|-------|----------|
| Memoria base | 15-25 MB | 10-20 MB |
| CPU en reposo | <0.1% | <0.1% |
| Binario | 2-3 MB | 1-2 MB |
| Dependencias | SQLite3, libcurl | Múltiples librerías |
| Lenguaje | C++ | C |
| Configuración | TOML | Configuración compleja |
| Plugins | 7 collectors básicos | 100+ plugins |
| HTTP endpoints | Nativo | Requiere plugin http |

**Ventajas de ChuspaPulse**: HTTP endpoints nativos, almacenamiento histórico integrado, configuración más moderna.
**Ventajas de Collectd**: Muy ligero, maduro, amplio ecosistema de plugins.

### Cuándo elegir ChuspaPulse

ChuspaPulse es la mejor opción cuando:
- Se necesita un agente ligero con mínimo consumo de recursos
- Se requiere almacenamiento histórico sin configurar una base de datos externa
- La configuración debe ser simple y mantenible
- El entorno tiene recursos limitados (VPS pequeños, embedded systems)
- Se prefiere C++ por su rendimiento y control de memoria

ChuspaPulse puede no ser la mejor opción cuando:
- Se requiere soporte para múltiples protocolos de salida (InfluxDB, Graphite, etc.)
- Se necesita un ecosistema amplio de plugins especializados
- La organización ya tiene una infraestructura basada en otra solución
- Se requieren features avanzadas de alertas y autenticación (no implementadas aún)
