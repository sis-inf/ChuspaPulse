#pragma once

#include "../config/config.hpp"
#include <filesystem>
#include <string>

namespace chuspapulse::cli {

/// Rango permitido para --interval, en milisegundos (ambos incluidos).
/// El minimo mantiene la regla documentada "mayor a 100 ms".
constexpr long long INTERVALO_MIN_MS = 101;
constexpr long long INTERVALO_MAX_MS = 3600000; // 1 hora

/// Archivo de configuracion por defecto, relativo al directorio de trabajo.
constexpr const char* CONFIG_POR_DEFECTO = "chuspapulse.toml";
/// Nombre historico del archivo de configuracion, aceptado por compatibilidad.
constexpr const char* CONFIG_HISTORICO = "pulso.toml";

/**
 * @brief Ruta inicial del archivo de configuracion TOML.
 *
 * Devuelve "chuspapulse.toml". Si ese archivo no existe pero si existe
 * "pulso.toml" (nombre historico), devuelve "pulso.toml" para no perder
 * la configuracion de instalaciones anteriores. La opcion --config <ruta>
 * reemplaza este valor.
 */
inline std::string config_path_por_defecto()
{
    std::error_code ec;
    if (!std::filesystem::exists(CONFIG_POR_DEFECTO, ec) &&
        std::filesystem::exists(CONFIG_HISTORICO, ec)) {
        return CONFIG_HISTORICO;
    }
    return CONFIG_POR_DEFECTO;
}

/**
 * @brief Opciones extendidas de linea de comandos.
 *
 * Incluye la configuracion de monitor mas flags nuevos
 * para el modo 'once' y formato de salida.
 */
struct CliOptions {
    chuspapulse::config::MonitorConfig monitor;
    bool once = false;
    std::string format = "json";
    /// Archivo TOML a cargar: "chuspapulse.toml" por defecto, con fallback a
    /// "pulso.toml" (ver config_path_por_defecto). Se cambia con --config.
    std::string config_path = config_path_por_defecto();
    /// true si --interval se paso por linea de comandos.
    bool interval_cli = false;
};

/**
 * @brief Procesa argumentos de linea de comandos.
 *
 * Argumentos soportados:
 * - --interval <ms>    (milisegundos, entero entre INTERVALO_MIN_MS e INTERVALO_MAX_MS)
 * - --metrics <lista>  (solo cpu, ram y disk, separados por coma)
 * - --once
 * - --format json|csv|prometheus|text
 * - --config <ruta>
 * - -h, --help
 * - --version
 *
 * @param argc Cantidad de argumentos.
 * @param argv Vector de argumentos.
 * @param options Configuracion extendida a modificar.
 *
 * @return true si la ejecucion puede continuar.
 * @return false si ocurrio un error o se mostro ayuda. Esto incluye
 *         una lista de --metrics vacia o con nombres desconocidos y un
 *         --interval que no sea un entero dentro del rango permitido.
 */
bool parse_arguments(int argc, char* argv[], CliOptions& options);

/**
 * @brief Calcula el intervalo de muestreo efectivo en milisegundos.
 *
 * Si --interval se paso por linea de comandos, prevalece ese valor.
 * En caso contrario se usa sampler.intervalo_segundos de la configuracion
 * (archivo TOML o variable de entorno) convertido a milisegundos.
 *
 * @param options Opciones obtenidas de parse_arguments.
 * @param config Configuracion cargada.
 *
 * @return Intervalo de muestreo en milisegundos.
 */
int resolver_intervalo_ms(const CliOptions& options,
                          const chuspapulse::config::Config& config);

/**
 * @brief Muestra la ayuda del programa.
 */
void print_help();

} // namespace chuspapulse::cli
