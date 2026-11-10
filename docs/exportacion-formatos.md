# Comparativa de formatos de exportación

## Resumen

ChuspaPulse permite exportar las métricas recolectadas en diferentes formatos para adaptarse a distintos casos de uso, desde la integración con sistemas de monitoreo hasta la generación de reportes o el análisis de datos.

Este documento compara los formatos soportados y ofrece ejemplos prácticos de cada uno.

---

# Tabla comparativa

| Formato | Caso de uso recomendado |
|----------|-------------------------|
| JSON | Integración con aplicaciones, APIs y procesamiento automático. |
| CSV | Importación en hojas de cálculo o herramientas de análisis de datos. |
| Texto | Consulta rápida desde la terminal o depuración. |
| Prometheus | Integración con Prometheus mediante scraping. |
| OpenMetrics | Integración con herramientas compatibles con OpenMetrics. |
| Influx | Envío de métricas a InfluxDB, Telegraf o VictoriaMetrics. |
| YAML | Configuración, intercambio de datos y lectura humana. |
| Markdown | Generación de reportes técnicos, documentación o Pull Requests. |

---

# Ejemplos de salida

## JSON

```json
{
  "cpu": 23.4,
  "memory": 61.2
}
```

---

## CSV

```csv
metrica,valor
cpu,23.4
memory,61.2
```

---

## Texto

```text
CPU: 23.4 %
Memoria: 61.2 %
```

---

## Prometheus

```text
ChuspaPulse_cpu_pct 23.4
ChuspaPulse_memory_pct 61.2
```

---

## OpenMetrics

```text
# TYPE ChuspaPulse_cpu_pct gauge
ChuspaPulse_cpu_pct 23.4

# TYPE ChuspaPulse_memory_pct gauge
ChuspaPulse_memory_pct 61.2

# EOF
```

---

## Influx

```text
ChuspaPulse cpu=23.4,memory=61.2
```

---

## YAML

```yaml
cpu: 23.4
memory: 61.2
```

---

## Markdown

```markdown
| Métrica | Valor |
|---------|------:|
| CPU | 23.4 % |
| Memoria | 61.2 % |
```

---

# ¿Qué formato elegir?

## JSON

Recomendado para:

- APIs.
- Automatización.
- Scripts.
- Integración con otras aplicaciones.

---

## CSV

Recomendado para:

- Microsoft Excel.
- LibreOffice Calc.
- Google Sheets.
- Herramientas de análisis de datos.

---

## Texto

Recomendado para:

- Inspección rápida.
- Consola.
- Depuración.

---

## Prometheus

Recomendado cuando ChuspaPulse será consultado directamente por Prometheus mediante scraping.

---

## OpenMetrics

Adecuado para plataformas compatibles con el estándar OpenMetrics y herramientas modernas de observabilidad.

---

## Influx

Recomendado para enviar métricas hacia:

- InfluxDB
- Telegraf
- VictoriaMetrics

---

## YAML

Útil cuando se requiere un formato fácilmente legible por personas y aplicaciones.

---

## Markdown

Especialmente útil para:

- Reportes técnicos.
- Documentación.
- Wikis.
- Pull Requests.
- Informes de resultados.

---

# Conclusiones

ChuspaPulse ofrece múltiples formatos de exportación para adaptarse a distintos escenarios de uso.

La elección del formato depende del destino de las métricas: JSON para automatización, CSV para análisis, Prometheus y OpenMetrics para monitoreo, Influx para plataformas de series temporales, YAML para intercambio de información, Markdown para documentación y texto para inspecciones rápidas.
