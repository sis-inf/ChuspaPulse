# Comparación de Formatos de Salida

> **Nota:** Este documento es una guía de decisión para elegir el formato correcto
> según tu caso de uso en tiempo real (modo `--once` o `--daemon`).
> Si buscas información sobre exportación masiva histórica desde Storage,
> consulta [docs/exportacion-formatos.md](exportacion-formatos.md).

---

## ¿Cuál formato debo usar?

### ¿Vas a consumir esto con un script?
Usa **json**.

### ¿Vas a abrirlo en Excel o Google Sheets?
Usa **csv**.

### ¿Tienes un scraper Prometheus, VictoriaMetrics u OpenMetrics?
Usa **prometheus** o **openmetrics**.

### ¿Vas a pegarlo en un PR o canal de chat?
Usa **markdown**.

### ¿Alimentas Telegraf o InfluxDB directamente?
Usa **influx**.

### ¿Solo quieres leerlo en la terminal?
Usa **text**.

---

## Guía de Decisión
¿Cuál es el destino de la salida?
│
├── Script / API / jq ──────────────────────→ json
├── Excel / Google Sheets / Pandas ─────────→ csv
├── Prometheus / Grafana / VictoriaMetrics ─→ prometheus / openmetrics
├── Telegraf / InfluxDB ─────────────────────→ influx
├── PR / Slack / canal de chat ──────────────→ markdown
└── Terminal (lectura rápida) ───────────────→ text

---

## Descripción de cada formato

### text
Salida legible para personas, ideal para inspección rápida en la terminal.
- **Cuándo usarlo:** quieres ver los datos de un vistazo sin procesarlos.
- **Comando:** `chuspapulse --format text --once`

### json
Salida estructurada en formato clave-valor.
- **Cuándo usarlo:** vas a procesar la salida con un script, API o herramienta como `jq`.
- **Comando:** `chuspapulse --format json --once`

### csv
Salida tabular separada por comas.
- **Cuándo usarlo:** vas a abrir los datos en Excel, Google Sheets o Python (Pandas).
- **Comando:** `chuspapulse --format csv --once`

### prometheus
Salida en formato de métricas para scraping.
- **Cuándo usarlo:** tienes un servidor Prometheus o Grafana recolectando métricas.
- **Comando:** `chuspapulse --format prometheus --daemon`

### openmetrics
Variante del formato Prometheus compatible con OpenMetrics.
- **Cuándo usarlo:** tu scraper requiere el estándar OpenMetrics en lugar de Prometheus clásico.
- **Comando:** `chuspapulse --format openmetrics --daemon`

### influx
Salida en formato de línea de InfluxDB.
- **Cuándo usarlo:** alimentas directamente Telegraf o InfluxDB.
- **Comando:** `chuspapulse --format influx --daemon`

### markdown
Salida en formato tabla Markdown.
- **Cuándo usarlo:** vas a pegar los datos en un PR, issue, Slack o canal de chat.
- **Comando:** `chuspapulse --format markdown --once`

---

## Tabla Comparativa

| Formato | Destino | Modo recomendado |
|---------|---------|-----------------|
| text | Terminal, lectura humana | `--once` |
| json | Scripts, APIs, `jq` | `--once` |
| csv | Excel, Sheets, Pandas | `--once` |
| prometheus | Prometheus, Grafana | `--daemon` |
| openmetrics | VictoriaMetrics, scrapers | `--daemon` |
| influx | Telegraf, InfluxDB | `--daemon` |
| markdown | PRs, Slack, chat | `--once` |