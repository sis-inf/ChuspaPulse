# Casos de Uso Avanzados

Escenarios completos end-to-end para uso de chuspapulse en entornos de produccion.

## Escenario 1: Alertar al equipo por Slack cuando el disco supera el 90%

Combina: monitoreo de disco + webhook de alertas.

Configuracion en chuspapulse.toml:

    [collectors]
    disk = true

    [alertas]
    habilitadas = true

    [[alertas.reglas]]
    metrica = "disk.used_percent"
    umbral = 90.0
    operador = ">"
    webhook = "https://hooks.slack.com/services/TU/WEBHOOK/URL"
    mensaje = "Disco supero el 90% en {hostname}"

Ejecutar chuspapulse:

    ./chuspapulse --config chuspapulse.toml

Cuando el disco supere el 90%, chuspapulse enviara automaticamente un mensaje al canal de Slack configurado.

## Escenario 2: Mantener 90 dias de historial sin que la base de datos crezca sin control

Combina: retencion de datos + downsampling.

Configuracion en chuspapulse.toml:

    [storage]
    habilitado = true
    ruta = "/var/lib/chuspapulse/datos.db"
    retencion_dias = 90
    downsampling = true
    downsampling_intervalo = "1h"

Con esta configuracion:
- Los datos de los ultimos 90 dias se conservan completos
- Los datos mas antiguos se comprimen a una muestra por hora
- La base de datos no crece indefinidamente

Verificar el tamano de la base de datos:

    du -sh /var/lib/chuspapulse/datos.db

## Escenario 3: Exportar metricas a un pipeline de Telegraf usando Influx Line Protocol

Combina: formato de exportacion InfluxDB + integracion con Telegraf.

Configuracion en chuspapulse.toml:

    [output]
    formato = "influx"
    destino = "http://localhost:8186/write"

Configuracion en Telegraf (telegraf.conf):

    [[inputs.http]]
    urls = ["http://localhost:8186/write"]
    data_format = "influx"

    [[outputs.influxdb_v2]]
    urls = ["http://localhost:8086"]
    token = "TU_TOKEN"
    organization = "mi-org"
    bucket = "chuspapulse"

Ejecutar chuspapulse y Telegraf:

    ./chuspapulse --config chuspapulse.toml &
    telegraf --config telegraf.conf

Las metricas de chuspapulse llegaran automaticamente a InfluxDB via Telegraf.

## Escenario 4: Monitorear una maquina de desarrollo sin exponer el agente a la red

Combina: modo watch local + sin servidor HTTP.

Configuracion en chuspapulse.toml:

    [servidor]
    habilitado = false

    [output]
    formato = "texto"
    destino = "stdout"

Ejecutar en modo watch local:

    ./chuspapulse --watch --config chuspapulse.toml

Con esta configuracion:
- chuspapulse muestra las metricas en la terminal en tiempo real
- No abre ningun puerto de red
- No expone el agente a conexiones externas
- Util para depuracion en maquinas de desarrollo
