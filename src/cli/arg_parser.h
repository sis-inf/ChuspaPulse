#pragma once

#include "../config/config.hpp"
#include <string>

namespace chuspapulse::cli {

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
    std::string config_path = "pulso.toml";
    /// true si --interval se paso por linea de comandos.
    bool interval_cli = false;
};

/**
 * @brief Procesa argumentos de linea de comandos.
 *
 * Argumentos soportados:
 * - --interval <ms>  (milisegundos, debe ser mayor a 100)
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
 *         una lista de --metrics vacia o con nombres desconocidos.
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
