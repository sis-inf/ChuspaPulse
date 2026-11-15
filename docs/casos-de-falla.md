# Casos de Falla

Comportamiento del agente ChuspaPulse ante condiciones de error y como recuperarse.

## Nota sobre verificacion

Los escenarios 1, 2 y 4 fueron verificados localmente reproduciendo las condiciones de falla. Los escenarios 3 y 5 estan documentados basandose en el comportamiento de SQLiteCpp y el sistema operativo Linux respectivamente.

## 1. Disco lleno — Storage no puede escribir

**Verificado:** Se lleno el disco simulando con un archivo grande y se observo el comportamiento.

### Que ocurre

Cuando el disco donde se encuentra chuspapulse.db se llena, la clase Storage lanza ErrorStorage al intentar insertar un nuevo snapshot. El agente se degrada parcialmente: sigue recolectando metricas y sirviendo /metrics con datos en memoria, pero el historial no se actualiza.

### Log observado

    [2026-06-10 14:32:11] [ERROR] Storage::save — SQLite error: database or disk is full
    [2026-06-10 14:32:11] [WARN]  El snapshot no fue persistido. Continuando en modo degradado.

### Como recuperarse

1. Verificar espacio en disco:

    df -h

2. Liberar espacio eliminando archivos innecesarios
3. Reiniciar chuspapulse:

    ./chuspapulse --config chuspapulse.toml

## 2. /proc inaccesible — collector de CPU o red falla

**Verificado:** Se restringieron permisos de /proc/stat y se ejecuto chuspapulse como usuario sin privilegios.

### Que ocurre

Los collectors de CPU y red leen de /proc/stat y /proc/net/dev. Si no son accesibles, el collector devuelve cero y el agente continua. Los otros collectors no se ven afectados.

### Log observado

    [2026-06-10 14:35:02] [WARN]  collector_cpu: no se pudo leer /proc/stat — permission denied (errno 13)
    [2026-06-10 14:35:02] [WARN]  collector_network: no se pudo leer /proc/net/dev — permission denied (errno 13)
    [2026-06-10 14:35:02] [INFO]  Snapshot guardado con metricas parciales (cpu=0, network=0)

### Como recuperarse

    sudo ./chuspapulse --config chuspapulse.toml

O en Docker:

    docker run --privileged ...

## 3. Base de datos corrupta o bloqueada

**Documentado segun comportamiento de SQLiteCpp:** ErrorStorage al abrir la base de datos.

### Que ocurre

Si chuspapulse.db esta corrupto o bloqueado por otro proceso, Storage lanza ErrorStorage al iniciar. ChuspaPulse no arranca.

### Log esperado

    [ERROR] Storage: no se pudo abrir la base de datos — database disk image is malformed
    [ERROR] Storage: no se pudo abrir la base de datos — database is locked

### Como recuperarse

Verificar integridad:

    sqlite3 chuspapulse.db "PRAGMA integrity_check;"

Si esta corrupto:

    cp chuspapulse.db chuspapulse.db.bak
    rm chuspapulse.db
    ./chuspapulse --config chuspapulse.toml

Si esta bloqueado:

    lsof chuspapulse.db

Detener el proceso y reiniciar chuspapulse.

## 4. Puerto HTTP ya en uso

**Verificado:** Se levanto otro proceso en el puerto 8080 y se intento iniciar chuspapulse.

### Que ocurre

Si el puerto configurado ya esta en uso, chuspapulse no puede iniciar el servidor HTTP y termina.

### Log observado

    [2026-06-10 14:40:15] [ERROR] servidor HTTP: bind() fallo en puerto 8080 — address already in use (errno 98)
    [2026-06-10 14:40:15] [FATAL] No se pudo iniciar el servidor. Terminando.

### Como recuperarse

Identificar el proceso:

    lsof -i :8080

Cambiar el puerto en chuspapulse.toml:

    [servidor]
    puerto = 9090

## 5. Permisos insuficientes para leer metricas del sistema

**Documentado segun comportamiento del kernel Linux:** acceso denegado a archivos de sistema.

### Que ocurre

Algunos collectors requieren permisos especiales. Si chuspapulse se ejecuta sin privilegios, algunos valores seran 0.

### Log esperado

    [WARN] collector_disk: no se pudo leer estadisticas de E/S — permission denied

### Como recuperarse

    sudo ./chuspapulse --config chuspapulse.toml

O usar setcap:

    sudo setcap cap_sys_ptrace+ep ./chuspapulse
