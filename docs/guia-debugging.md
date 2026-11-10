# Guía de depuración de ChuspaPulse con GDB y Valgrind

## Introducción

Durante el desarrollo de ChuspaPulse pueden presentarse errores como cierres inesperados, fugas de memoria o comportamientos incorrectos. Esta guía explica cómo compilar el proyecto en modo depuración y utilizar herramientas comunes para diagnosticar estos problemas.

---

# Compilar en modo Debug

Para generar un ejecutable con símbolos de depuración utilice:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug .
make
```

El parámetro `CMAKE_BUILD_TYPE=Debug` permite incluir información de depuración para que herramientas como GDB puedan mostrar nombres de funciones y líneas de código.

---

# Depuración con GDB

Ejecute el programa con GDB:

```bash
gdb ./ChuspaPulse
```

Inicie la ejecución:

```gdb
run
```

Si el programa finaliza por un fallo (por ejemplo, un *segmentation fault*), obtenga el backtrace con:

```gdb
backtrace
```

o de forma abreviada:

```gdb
bt
```

El backtrace muestra la secuencia de llamadas que llevó el error y facilita identificar la función donde ocurrió el problema.

Para salir de GDB:

```gdb
quit
```

---

# Detección de fugas de memoria con Valgrind

Valgrind permite detectar pérdidas de memoria y accesos inválidos.

Ejecute:

```bash
valgrind --leak-check=full ./ChuspaPulse
```

Para obtener información más detallada:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./ChuspaPulse
```

Esta herramienta resulta especialmente útil para revisar componentes que administran recursos, como:

- Storage
- Sampler
- Collectors
- Conexiones SQLite
- Hilos de ejecución

---

# Activar el nivel DEBUG del Logger

Para obtener información detallada durante la ejecución configure el Logger con nivel **DEBUG**.

Ejemplo:

```text
log.level=DEBUG
```

o mediante la configuración correspondiente del proyecto.

Al habilitar este nivel se registran mensajes adicionales sobre:

- Inicio del monitoreo.
- Recolección de métricas.
- Evaluación de alertas.
- Escritura en almacenamiento.
- Errores internos.

Esta información facilita localizar la causa de un comportamiento inesperado.

---

# Flujo recomendado de depuración

1. Compilar el proyecto en modo Debug.
2. Ejecutar el programa normalmente y reproducir el problema.
3. Si ocurre un cierre inesperado, analizar el backtrace con GDB.
4. Ejecutar Valgrind para comprobar pérdidas de memoria.
5. Activar el nivel DEBUG del Logger para obtener mayor contexto durante la ejecución.

---

# Buenas prácticas

- Compilar siempre en modo Debug durante el desarrollo.
- Revisar el backtrace antes de modificar el código.
- Verificar con Valgrind que no existan fugas de memoria.
- Utilizar los mensajes del Logger para complementar el diagnóstico.
