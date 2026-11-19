# Benchmark de Collectors (ChuspaPulse bench)

Documentacion del subcomando bench para medir el overhead de los collectors.

## Comando

    ./ChuspaPulse bench --iteraciones N

Donde N es el numero de iteraciones a ejecutar para calcular el promedio. Se recomienda usar al menos 100 iteraciones para obtener resultados estables.

Ejemplo:

    ./ChuspaPulse bench --iteraciones 100

## Ejemplo de salida

    Benchmark de collectors - 100 iteraciones
    ==========================================
    Collector       Promedio (ms)   Min (ms)   Max (ms)
    ----------------------------------------------------
    cpu             1.23            0.98       2.41
    memory          0.45            0.31       0.89
    disk            3.12            2.87       4.23
    network         0.78            0.61       1.02
    ----------------------------------------------------
    Total           5.58            4.77       8.55

    Intervalo minimo recomendado: 6 segundos

## Como interpretar los resultados

### Total

El valor Total representa el tiempo que ChuspaPulse necesita para ejecutar todos los collectors en un ciclo de muestreo. Este valor es el mas importante para configurar el intervalo.

### Intervalo minimo recomendado

La regla general es que el intervalo de muestreo debe ser al menos 2x el tiempo total de los collectors para evitar que un ciclo se superponga con el siguiente.

    intervalo_minimo = total_promedio * 2

En el ejemplo anterior: 5.58ms * 2 = ~12ms, lo que en la practica se redondea al segundo entero mas cercano.

## Configurar el intervalo segun los resultados

Una vez obtenidos los resultados del bench, ajustar intervalo_segundos en ChuspaPulse.toml:

    [sampler]
    intervalo_segundos = 10

Guia de referencia:

| Total promedio del bench | Intervalo recomendado |
|---|---|
| Menos de 100ms | 5 segundos |
| Entre 100ms y 500ms | 10 segundos |
| Entre 500ms y 1000ms | 15 segundos |
| Mas de 1000ms | 30 segundos o mas |

## Casos de uso

### Maquina con recursos limitados

En maquinas con poco CPU o disco lento, los collectors de disk y network pueden tardar mas. Ejecutar el bench en la maquina objetivo:

    ./ChuspaPulse bench --iteraciones 200

Si el total supera los 500ms, usar un intervalo de al menos 15 segundos:

    [sampler]
    intervalo_segundos = 15

### Maquina de alta disponibilidad

En servidores donde se necesita mayor frecuencia de muestreo, verificar que el bench muestre un total menor a 100ms antes de configurar intervalos cortos:

    [sampler]
    intervalo_segundos = 5

### Identificar collectors lentos

Si el bench muestra que un collector especifico es lento (por ejemplo disk con mas de 5ms), se puede desactivar ese collector en ChuspaPulse.toml para reducir el overhead total.
