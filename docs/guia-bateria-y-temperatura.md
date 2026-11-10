# Guía de Collectors de Batería y Temperatura

## Resumen

ChuspaPulse incorpora collectors para obtener información de la **batería** y de los **sensores de temperatura** del sistema.

Estos collectors tienen un comportamiento especial: cuando el hardware no proporciona esta información, **retornan un resultado vacío sin generar errores**.

Este comportamiento es **esperado** y **no representa un fallo del sistema**.

---

# ¿Por qué algunos equipos no muestran datos?

No todos los equipos exponen información sobre batería o temperatura mediante las interfaces del sistema operativo.

Es común que:

- Servidores físicos no tengan batería.
- Máquinas virtuales no expongan sensores térmicos.
- Contenedores no tengan acceso al hardware del host.
- Algunos sistemas embebidos limiten el acceso a estos dispositivos.

En estas situaciones, el collector devuelve un resultado vacío porque simplemente no existe información disponible para consultar.

---

# Este comportamiento no es un bug

Cuando el collector devuelve un vector vacío:

- No significa que ChuspaPulse haya fallado.
- No indica un error interno.
- No requiere reiniciar el agente.

Simplemente significa que el hardware o el sistema operativo no ofrecen dichos datos.

---

# Verificar soporte para batería

Antes de reportar un problema, es recomendable comprobar si el sistema expone información de batería.

Ejecutar:

```bash
ls /sys/class/power_supply/
```

Si existe un dispositivo similar a:

```text
BAT0
```

o

```text
BAT1
```

el sistema dispone de información sobre batería.

También puede consultarse:

```bash
cat /sys/class/power_supply/BAT0/capacity
```

Ejemplo:

```text
87
```

---

# Verificar soporte para temperatura

Para comprobar los sensores térmicos disponibles:

```bash
ls /sys/class/thermal/
```

En sistemas compatibles aparecerán entradas como:

```text
thermal_zone0
thermal_zone1
thermal_zone2
```

Para consultar una temperatura:

```bash
cat /sys/class/thermal/thermal_zone0/temp
```

Ejemplo:

```text
47000
```

El valor normalmente se expresa en milésimas de grado Celsius.

En este ejemplo:

```
47000 = 47 °C
```

---

# Ejemplo en una laptop

En un equipo portátil compatible el collector puede producir información similar a:

```text
Batería
--------

Nivel:
92 %

Estado:
Discharging

Temperatura
-----------

CPU:
47 °C

GPU:
43 °C

SSD:
36 °C
```

---

# ¿Cuándo reportar un problema?

Es recomendable abrir un reporte únicamente cuando:

- El hardware sí expone la información mediante `/sys/class/power_supply/`.
- El hardware sí expone sensores en `/sys/class/thermal/`.
- ChuspaPulse continúa devolviendo resultados vacíos.

Si el sistema operativo no ofrece estos dispositivos, el comportamiento esperado del collector es retornar un resultado vacío.

---

# Conclusiones

Los collectors de batería y temperatura están diseñados para funcionar en diferentes tipos de hardware.

Cuando los dispositivos o sensores no existen, el collector retorna un resultado vacío sin producir errores. Esto permite que ChuspaPulse continúe monitoreando el resto del sistema sin interrumpir la ejecución del agente.