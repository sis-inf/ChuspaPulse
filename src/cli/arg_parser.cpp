#include "arg_parser.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <fstream>

#define CHUSPAPULSE_VERSION "0.1.0"

namespace chuspapulse::cli {

/**
 * @brief Procesa la lista de metricas y actualiza la configuracion.
 *
 * Ejemplo:
 * --metrics cpu,ram
 *
 * @return false si la lista esta vacia o contiene nombres desconocidos.
 *         En ese caso la configuracion no se modifica.
 */
static bool parse_metrics(const std::string& metrics, chuspapulse::config::MonitorConfig& config)
{
    if (metrics.empty())
    {
        std::cerr << "Error: la lista de --metrics esta vacia\n";
        std::cerr << "Metricas validas: cpu, ram, disk\n";
        return false;
    }

    bool cpu = false;
    bool ram = false;
    bool disk = false;
    std::string desconocidas;

    std::stringstream ss(metrics);
    std::string item;

    while (std::getline(ss, item, ','))
    {
        if (item == "cpu")
        {
            cpu = true;
        }
        else if (item == "ram")
        {
            ram = true;
        }
        else if (item == "disk")
        {
            disk = true;
        }
        else
        {
            if (!desconocidas.empty())
            {
                desconocidas += ", ";
            }
            desconocidas += item.empty() ? "\"\"" : item;
        }
    }

    if (metrics.back() == ',')
    {
        if (!desconocidas.empty())
        {
            desconocidas += ", ";
        }
        desconocidas += "\"\"";
    }

    if (!desconocidas.empty())
    {
        std::cerr << "Error: metricas desconocidas: " << desconocidas << "\n";
        std::cerr << "Metricas validas: cpu, ram, disk\n";
        return false;
    }

    config.cpu = cpu;
    config.ram = ram;
    config.disk = disk;
    return true;
}

void print_help()
{
    std::cout << "Uso:\n";
    std::cout << "  chuspapulse [opciones]\n\n";

    std::cout << "Opciones:\n";

    std::cout << "  --interval <ms>   Intervalo de muestreo en milisegundos ("
              << INTERVALO_MIN_MS << " a " << INTERVALO_MAX_MS << ")\n";
    std::cout << "  --metrics <list>  Metricas a recolectar: cpu,ram,disk\n";
    std::cout << "  --once            Ejecutar una sola lectura y salir\n";
    std::cout << "  --format <fmt>    Formato de salida con --once: json|csv|prometheus|text\n";
    std::cout << "  -h, --help        Mostrar ayuda\n";
    std::cout << "  --version         Mostrar version\n";
}

bool parse_arguments(int argc, char* argv[], chuspapulse::cli::CliOptions& options)
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            print_help();
            return false;
        }
        else if (arg == "--version")
        {
            std::cout << "chuspapulse v"
                      << CHUSPAPULSE_VERSION
                      << " (C++17) - 2026\n";

            std::exit(0);
        }
        else if (arg == "--interval")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: falta valor para --interval\n";
                return false;
            }

            const std::string valor = argv[++i];

            // Validar antes de convertir: solo digitos (sin signo, espacios
            // ni sufijos).
            const bool soloDigitos =
                !valor.empty() &&
                valor.find_first_not_of("0123456789") == std::string::npos;

            if (!soloDigitos)
            {
                std::cerr << "Error: --interval debe ser un entero positivo en "
                             "milisegundos (recibido: '" << valor << "')\n";
                return false;
            }

            // Mas de 10 digitos ya esta fuera de rango; se descarta antes de
            // convertir para que stoll no desborde.
            const long long interval =
                valor.size() <= 10 ? std::stoll(valor) : INTERVALO_MAX_MS + 1;

            if (interval < INTERVALO_MIN_MS || interval > INTERVALO_MAX_MS)
            {
                std::cerr << "Error: --interval fuera de rango: " << valor
                          << " ms (permitido: " << INTERVALO_MIN_MS << " a "
                          << INTERVALO_MAX_MS << " ms)\n";
                return false;
            }

            options.monitor.interval_ms = static_cast<int>(interval);
            options.interval_cli = true;
        }
        else if (arg == "--metrics")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: falta valor para --metrics\n";
                return false;
            }

            if (!parse_metrics(argv[++i], options.monitor))
            {
                return false;
            }
        }
        else if (arg == "--once")
        {
            options.once = true;
        }
        else if (arg == "--format")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: falta valor para --format\n";
                return false;
            }
            std::string fmt = argv[++i];
            if (fmt != "json" && fmt != "csv" && fmt != "prometheus" && fmt != "text")
            {
                std::cerr << "Error: formato no soportado: " << fmt << "\n";
                std::cerr << "Formatos validos: json, csv, prometheus, text\n";
                return false;
            }
            options.format = fmt;
        }
        else if (arg == "--config")
        {
            if (i + 1 >= argc || std::string(argv[i + 1]).rfind("-", 0) == 0)
            {
                std::cerr << "Error: falta ruta para --config\n";
                std::cerr << "Uso: --config <ruta>\n";
                return false;
            }

            std::string ruta = argv[++i];

            if (!std::filesystem::is_regular_file(ruta))
            {
                std::cerr << "Error: el archivo de configuracion no existe: "
                          << ruta << "\n";
                return false;
            }

            std::ifstream archivo(ruta);
            if (!archivo.is_open())
            {
                std::cerr << "Error: no se puede leer el archivo de configuracion: "
                          << ruta << "\n";
                return false;
            }

            options.config_path = ruta;
        }
        else
        {
            std::cerr << "Error: argumento desconocido -> "
                      << arg << "\n";

            return false;
        }
    }

    return true;
}

int resolver_intervalo_ms(const CliOptions& options,
                          const chuspapulse::config::Config& config)
{
    if (options.interval_cli)
    {
        return options.monitor.interval_ms;
    }

    return config.sampler.intervalo_segundos * 1000;
}

} // namespace chuspapulse::cli
