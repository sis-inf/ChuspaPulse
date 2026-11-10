
# Troubleshooting de ChuspaPulse

## 1. Permission denied al leer /proc/stat

### Síntoma

ChuspaPulse muestra errores al intentar leer métricas de CPU.

### Causa

El usuario no tiene permisos suficientes para acceder a archivos del sistema.

### Solución

```bash
ls -l /proc/stat
sudo ./ChuspaPulse
```
---

## 2. Puerto ya en uso

### Síntoma

La aplicación no inicia el servidor HTTP.

### Causa

Otro proceso está utilizando el mismo puerto.

### Solución

```bash
sudo lsof -i :8080
sudo kill -9 PID
```
---

## 3. CPU siempre reporta 0%

### Síntoma

Las métricas de CPU muestran 0% constantemente.

### Causa

No existe suficiente intervalo entre mediciones.

### Solución

```bash
./ChuspaPulse --once
sleep 2
./ChuspaPulse --once
```

---

## 4. RAM reportada incorrectamente

### Síntoma

El uso de memoria no coincide con herramientas del sistema.

### Causa

Diferencias entre memoria usada, libre y caché.

### Solución

```bash
free -h
cat /proc/meminfo
```
---

## 5. El agente no inicia como servicio

### Síntoma

El servicio falla al arrancar.

### Causa

Configuración incorrecta del servicio.

### Solución

```bash
sudo systemctl status ChuspaPulse
sudo journalctl -u ChuspaPulse
```
---

## 6. Posible memory leak

### Síntoma

El consumo de memoria aumenta continuamente.

### Causa

Objetos o recursos no liberados.

### Solución

```bash
valgrind ./ChuspaPulse
```

Nota: un incremento pequeño por cachés internas puede ser normal.

---

## 7. /sys/class/thermal no existe

### Síntoma

No se muestran métricas de temperatura.

### Causa

El sistema operativo o hardware no expone sensores térmicos.

### Solución

```bash
ls /sys/class/thermal
sensors
```

---

## 8. No se generan métricas Prometheus

### Síntoma

Prometheus no recibe datos.

### Causa

Formato incorrecto o endpoint no disponible.

### Solución

```bash
curl http://localhost:8080/metrics
```

---

## 9. Error al compilar

### Síntoma

La compilación falla.

### Causa

Dependencias faltantes.

### Solución

```bash
cmake -S . -B build
cmake --build build
```

---

## 10. Tests fallan

### Síntoma

Las pruebas automáticas muestran errores.

### Causa

Cambios incompatibles o compilación incompleta.

### Solución

```bash
cd build
ctest --output-on-failure
```
---
