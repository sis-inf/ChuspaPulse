#pragma once

#include "../config/config.hpp"
#include <string>

namespace chuspapulse::cli {

/// Rango permitido para --interval, en milisegundos (ambos incluidos).
/// El minimo mantiene la regla documentada "mayor a 100 ms".
constexpr long long INTERVALO_MIN_MS = 101;
constexpr long long INTERVALO_MAX_MS = 3600000; // 1 hora

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
};

/**
 * @brief Procesa argumentos de linea de comandos.
 *
 * Argumentos soportados:
 * - --interval <ms>    (entero entre INTERVALO_MIN_MS e INTERVALO_MAX_MS)
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
 * @brief Muestra la ayuda del programa.
 */
void print_help();

} // namespace chuspapulse::cli
