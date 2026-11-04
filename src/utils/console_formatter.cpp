#include "console_formatter.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

/** @brief Ancho total de la tabla en caracteres (borde a borde). */
static constexpr int ANCHO_TOTAL   = 60;

/** @brief Ancho de la columna "Métrica" (sin bordes ni espaciado). */
static constexpr int ANCHO_METRICA = 14;

/** @brief Ancho de la columna "Valor" (sin bordes ni espaciado). */
static constexpr int ANCHO_VALOR   = 18;

/** @brief Ancho total de la columna "Uso" (incluye espacios de relleno). */
static constexpr int ANCHO_BARRA   = 20;

/**
 * @brief Número de caracteres '#' o '-' dentro de los corchetes de la barra.
 *
 * La barra resultante tiene la forma `[##...--...]`, con una longitud de
 * BARRA_RELLENO + 2 caracteres (los dos corchetes).
 */
static constexpr int BARRA_RELLENO = 18;

// Helpers de conversión de unidades
/**
 * @brief Convierte bytes a gigabytes.
 * @param bytes  Valor en bytes.
 * @return       Equivalente en GB con precisión de punto flotante.
 */
static double aGB(double bytes) {
    return bytes / (1024.0 * 1024.0 * 1024.0);
}

/**
 * @brief Convierte bytes a kilobytes.
 * @param bytes  Valor en bytes.
 * @return       Equivalente en KB con precisión de punto flotante.
 */
static double aKB(double bytes) {
    return bytes / 1024.0;
}

// Helpers de construcción de la tabla
/**
 * @brief Genera una barra de progreso ASCII de ancho fijo.
 *
 * La barra tiene el formato `[####------]` seguido de espacios hasta
 * completar exactamente ANCHO_BARRA caracteres.
 *
 * Ejemplos:
 * - `pct = 0`   → `[------------------]`
 * - `pct = 50`  → `[#########---------]`
 * - `pct = 100` → `[##################]`
 *
 * @param pct  Porcentaje de uso (0–100). Valores fuera de rango se recortan.
 * @return     String de exactamente ANCHO_BARRA caracteres.
 */
static std::string barraProgreso(double pct) {
    const double p     = pct < 0.0 ? 0.0 : (pct > 100.0 ? 100.0 : pct);
    const int llenos   = static_cast<int>(std::round(p / 100.0 * BARRA_RELLENO));
    const int vacios   = BARRA_RELLENO - llenos;

    std::string barra  = "[";
    barra += std::string(llenos, '#');
    barra += std::string(vacios, '-');
    barra += "]";

    // Rellenar con espacios hasta ANCHO_BARRA para mantener columna fija
    if (static_cast<int>(barra.size()) < ANCHO_BARRA) {
        barra += std::string(ANCHO_BARRA - static_cast<int>(barra.size()), ' ');
    }
    return barra;
}

/**
 * @brief Construye una fila de la tabla con tres columnas de ancho fijo.
 *
 * Formato resultante:
 * @code
 * | <metrica padded> | <valor right-aligned> | <uso padded> |
 * @endcode
 *
 * @param metrica  Texto de la columna izquierda (nombre de la métrica).
 * @param valor    Texto de la columna central (valor formateado).
 * @param uso      Texto de la columna derecha (barra de progreso u otros).
 *                 Por defecto vacío.
 * @return         String con la fila completa incluyendo bordes `|`.
 */
static std::string fila(const std::string& metrica,
                        const std::string& valor,
                        const std::string& uso = "") {
    std::ostringstream oss;
    oss << "| "
        << std::left  << std::setw(ANCHO_METRICA) << metrica
        << "| "
        << std::right << std::setw(ANCHO_VALOR - 1) << valor << " "
        << "| "
        << std::left  << std::setw(ANCHO_BARRA) << uso
        << "|";
    return oss.str();
}

/**
 * @brief Retorna la línea de borde superior/inferior de la tabla.
 *
 * Ejemplo: `============================================================`
 *
 * @return String de ANCHO_TOTAL caracteres `=`.
 */
static std::string borde() {
    return std::string(ANCHO_TOTAL, '=');
}

/**
 * @brief Retorna la línea separadora entre la cabecera y las filas de datos.
 *
 * Ejemplo: `|---------------+-------------------+---------------------|`
 *
 * @return String con el separador interno usando `-` y `+`.
 */
static std::string separador() {
    std::string s = "|";
    s += std::string(ANCHO_METRICA + 1, '-');
    s += "+";
    s += std::string(ANCHO_VALOR + 1, '-');
    s += "+";
    s += std::string(ANCHO_BARRA + 1, '-');
    s += "|";
    return s;
}

// Implementación de la API pública
/**
 * @brief Formatea un snapshot como tabla ASCII para la terminal.
 *
 * Flujo interno:
 *  1. Recorre `snap.metricas` y extrae los 7 campos relevantes por nombre.
 *  2. Calcula el porcentaje de uso de RAM y Disco (usado/total × 100).
 *  3. Formatea cada valor en la unidad adecuada (%, GB, KB/s).
 *  4. Ensambla la tabla con borde, cabecera, separador y cuatro filas.
 *
 * @param snap  Snapshot de métricas del agente ChuspaPulse.
 * @return      Tabla ASCII lista para imprimir, terminada en `\n`.
 */
std::string formatAsTable(const chuspapulse::core::Snapshot& snap) {

    // ── 1. Extraer métricas del snapshot ─────────────────────────────────────

    double cpu_pct     = 0.0;  ///< Porcentaje de uso de CPU (0–100)
    double ram_usado   = 0.0;  ///< RAM utilizada en bytes
    double ram_total   = 0.0;  ///< RAM total en bytes
    double disco_usado = 0.0;  ///< Espacio de disco usado en bytes
    double disco_total = 0.0;  ///< Espacio de disco total en bytes
    double net_rx      = 0.0;  ///< Tráfico de red entrante en bytes
    double net_tx      = 0.0;  ///< Tráfico de red saliente en bytes

    for (const auto& m : snap.metricas) {
        if      (m.nombre == "cpu.usage"  || m.nombre == "cpu")          cpu_pct     = m.valor;
        else if (m.nombre == "ram.used"   || m.nombre == "ram.usado")    ram_usado   = m.valor;
        else if (m.nombre == "ram.total")                                 ram_total   = m.valor;
        else if (m.nombre == "disk.used"  || m.nombre == "disco.usado")  disco_usado = m.valor;
        else if (m.nombre == "disk.total" || m.nombre == "disco.total")  disco_total = m.valor;
        else if (m.nombre == "network.rx" || m.nombre == "net.rx")       net_rx      = m.valor;
        else if (m.nombre == "network.tx" || m.nombre == "net.tx")       net_tx      = m.valor;
    }

    // ── 2. Calcular porcentajes para las barras de progreso ───────────────────

    const double ram_pct   = (ram_total   > 0.0) ? (ram_usado   / ram_total   * 100.0) : 0.0;
    const double disco_pct = (disco_total > 0.0) ? (disco_usado / disco_total * 100.0) : 0.0;

    // ── 3. Formatear strings de valor para cada fila ──────────────────────────

    std::ostringstream v;
    v << std::fixed;

    // CPU: "45.3 %"
    v << std::setprecision(1) << cpu_pct << " %";
    const std::string val_cpu = v.str(); v.str(""); v.clear();

    // RAM: "3.2 / 8.0 GB"
    v << std::setprecision(1) << aGB(ram_usado) << " / " << aGB(ram_total) << " GB";
    const std::string val_ram = v.str(); v.str(""); v.clear();

    // Disco: "120.0 / 500.0 GB"
    v << std::setprecision(1) << aGB(disco_usado) << " / " << aGB(disco_total) << " GB";
    const std::string val_disco = v.str(); v.str(""); v.clear();

    // Red: "1.2 / 0.8 KB/s"
    v << std::setprecision(1) << aKB(net_rx) << " / " << aKB(net_tx) << " KB/s";
    const std::string val_red = v.str(); v.str(""); v.clear();

    // ── 4. Ensamblar la tabla ─────────────────────────────────────────────────

    std::ostringstream tabla;

    tabla << borde()     << "\n";
    tabla << fila("Métrica", "Valor", "Uso") << "\n";
    tabla << separador() << "\n";
    tabla << fila("CPU",      val_cpu,   barraProgreso(cpu_pct))   << "\n";
    tabla << fila("RAM",      val_ram,   barraProgreso(ram_pct))   << "\n";
    tabla << fila("Disco",    val_disco, barraProgreso(disco_pct)) << "\n";
    tabla << fila("Red  ↓↑", val_red,   "")                       << "\n";
    tabla << borde()     << "\n";

    return tabla.str();
}

// ── Función legacy ────────────────────────────────────────────────────────────

/**
 * @brief Imprime métricas clave-valor en `std::cout`.
 *
 * Formato por línea: `NOMBRE:   VALOR%`
 * Las entradas con valor negativo se omiten silenciosamente.
 *
 * @param data  Mapa nombre → valor numérico (porcentaje).
 *
 * @deprecated Preferir formatAsTable() para una presentación más completa.
 */
void printMetrics(const std::map<std::string, double>& data) {
    for (const auto& entry : data) {
        if (entry.second >= 0) {
            std::cout << std::left  << std::setw(6) << entry.first + ":"
                      << std::right << std::setw(8) << entry.second << "%\n";
        }
    }
}
