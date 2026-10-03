# Caso de Prueba: Overhead del Sampler
- Objetivo: Medir el impacto en el rendimiento (overhead) del sampler de métricas bajo carga sostenida.Precondiciones:
- Sistema operativo con entorno de compilación listo.
- Monitoreo de CPU/RAM disponible (ej. htop, Task Manager).
## Pasos:
1. Iniciar el proceso pulso sin el sampler activo y registrar el consumo base de CPU/RAM.
2. Activar el sampler con un intervalo de muestreo de 1 segundo.
3. Mantener el proceso en ejecución durante 10 minutos.
4. Registrar el consumo máximo y promedio de CPU/RAM del proceso.
## Criterio de Éxito:
El proceso no excede el 5% de uso de CPU bajo carga sostenida.
No se detectan fugas de memoria (el consumo de RAM se estabiliza después de los primeros minutos).