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
 * @return true si la ejecucion puede continuar.
 * @return false si ocurrio un error o se mostro ayuda.
 */
bool parse_arguments(int argc, char* argv[], CliOptions& options);

/**
 * @brief Muestra la ayuda del programa.
 */
void print_help();

} // namespace pulso::cli