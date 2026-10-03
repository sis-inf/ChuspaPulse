#include "arg_parser.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#define PULSO_VERSION "0.1.0"

/**
 * @brief Procesa la lista de metricas y actualiza la configuracion.
 *
 * Ejemplo:
 * --metrics cpu,ram
 */
static void parse_metrics(const std::string& metrics, pulso::config::MonitorConfig& config)
{
    config.cpu = false;
    config.ram = false;
    config.disk = false;

    std::stringstream ss(metrics);
    std::string item;

    while (std::getline(ss, item, ','))
    {
        if (item == "cpu")
        {
            config.cpu = true;
        }
        else if (item == "ram")
        {
            config.ram = true;
        }
        else if (item == "disk")
        {
            config.disk = true;
        }
    }
}

void print_help()
{
    std::cout << "Uso:\n";
    std::cout << "  pulso [opciones]\n\n";

    std::cout << "Opciones:\n";

    std::cout << "  --interval <ms>   Intervalo de lectura\n";
    std::cout << "  --metrics <list>  Metricas a recolectar: cpu,ram,disk\n";
    std::cout << "  --once            Ejecutar una sola lectura y salir\n";
    std::cout << "  --format <fmt>    Formato de salida con --once: json|csv|prometheus|text\n";
    std::cout << "  -h, --help        Mostrar ayuda\n";
    std::cout << "  --version         Mostrar version\n";
}

bool parse_arguments(int argc, char* argv[], pulso::cli::CliOptions& options)
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
            std::cout << "pulso v"
                      << PULSO_VERSION
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

            int interval = std::stoi(argv[++i]);

            if (interval <= 100)
            {
                std::cerr << "Error: interval debe ser mayor a 100ms\n";
                return false;
            }

            options.monitor.interval_ms = interval;
        }
        else if (arg == "--metrics")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: falta valor para --metrics\n";
                return false;
            }

            parse_metrics(argv[++i], options.monitor);
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
            if (fmt != "json" && fmt != "csv" && fmt != "prometheus")
            {
                std::cerr << "Error: formato no soportado: " << fmt << "\n";
                std::cerr << "Formatos validos: json, csv, prometheus\n";
                return false;
            }
            options.format = fmt;
        }
        else if (arg == "--config")
        {
            if (i + 1 < argc) ++i;
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