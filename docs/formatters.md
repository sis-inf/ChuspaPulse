# Guía de Formatos de Salida (Formatters)

> **Nota:** Este documento describe los formatos disponibles y ejemplos de salida.
> Si buscas una guía de decisión sobre cuándo usar cada formato,
> consulta [docs/comparacion-formatos-salida.md](comparacion-formatos-salida.md).

ChuspaPulse soporta múltiples formatos de salida para adaptarse a diferentes flujos de trabajo, desde la lectura rápida por parte de usuarios hasta la integración automática con sistemas de monitoreo o análisis de datos.


## Tabla Comparativa

| Formato | Caso de Uso Principal | Ventaja |
| :--- | :--- | :--- |
| **Text** | Inspección rápida directamente en la terminal. | Altamente legible para humanos. |
| **JSON** | Integración con scripts, APIs o herramientas como `jq`. | Estructurado y universalmente compatible. |
| **Prometheus** | Recolección de métricas (*scraping*) por PromQL / Grafana. | Listo para sistemas de monitoreo modernos. |
| **CSV** | Análisis de datos en Excel, Google Sheets o Python (Pandas). | Compatible con hojas de cálculo y análisis estadístico. |

## Formatos Soportados (Ejemplos de Salida)

### 1. Text
Salida legible para personas, ideal para visualizar información directamente en la terminal.
CPU: 25%
RAM: 60%
Disk: 45%
### 2. JSON
Salida estructurada en formato clave-valor, útil para scripts e integraciones.
{
  "cpu": 25,
  "ram": 60,
  "disk": 45
}
### 3. Prometheus
Salida en formato de métricas para herramientas de monitoreo como Prometheus y Grafana.
ChuspaPulse_cpu_percent 25
ChuspaPulse_ram_percent 60
ChuspaPulse_disk_percent 45
### 4. CSV
Salida tabular separada por comas, compatible con Excel y hojas de cálculo.
cpu,ram,disk
25,60,45
