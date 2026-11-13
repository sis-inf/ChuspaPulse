#pragma once

#include "../config/config.hpp"
#include <string>

namespace pulso::cli {

/**
 * @brief Opciones extendidas de linea de comandos.
 *
 * Incluye la configuracion de monitor mas flags nuevos
 * para el modo 'once' y formato de salida.
 */
struct CliOptions {
    pulso::config::MonitorConfig monitor;
    bool once = false;
    std::string format = "json";
    bool help_requested = false;
};

/**
 * @brief Procesa argumentos de linea de comandos.
 *
 * Argumentos soportados:
 * - --interval <ms>
 * - --metrics <lista>
 * - --once
 * - --format json|csv|prometheus|text
 * - -h, --help
 * - --version
 *
 * @param argc Cantidad de argumentos.
 * @param argv Vector de argumentos.
 * @param options Configuracion extendida a modificar.
 *
 * @return true si la ejecucion puede continuar o si se mostro la ayuda
 *         (en ese caso options.help_requested es true y el llamador debe
 *         terminar con codigo 0 sin iniciar el muestreo ni el servidor).
 * @return false si ocurrio un error de argumentos.
 */
bool parse_arguments(int argc, char* argv[], CliOptions& options);

/**
 * @brief Muestra la ayuda del programa.
 */
void print_help();

} // namespace pulso::cli