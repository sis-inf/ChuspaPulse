#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <ctime>

// httplib
#include <httplib.h>

// Config
#include "config/config.hpp"

// Logger
#include "utils/logging/logger.hpp"

// Storage
#include "storage/schema.hpp"
#include "storage/storage.hpp"

// Collectors
#include "collectors/memory/ram_usage.hpp"
#include "collectors/bateria/bateria_collector.hpp"
#include "platform/linux/collector_cpu.hpp"
// TODO: CollectorDisk y CollectorNetwork no implementan ICollector aún.
//       Pendiente en issue separado.

// Sampler
#include "sampler/sampler.hpp"

// Formatters (OFICIALES del proyecto)
#include "formatters/formatter_json.hpp"
#include "formatters/formatter_csv.hpp"
#include "formatters/formatter_prometheus.hpp"

// HTTP handlers
#include "http/handler_config.hpp"
#include "http/handler_health.hpp"
#include "http/handler_history.hpp"
#include "http/handler_metrics.hpp"
#include "http/handler_prometheus.hpp"
#include "http/handler_version.hpp"

// CLI
#include "cli/arg_parser.h"

// Core types
#include "core/types.hpp"

// Signal handler
extern std::atomic<bool> isRunning;
void setupSignalHandler();

// ============================================
// NUEVO: Funcion helper para modo once
// ============================================

/**
 * @brief Ejecuta una sola coleccion de metricas y las imprime a stdout.
 *
 * Usa los formatters OFICIALES del proyecto (JSON, CSV, Prometheus).
 */
static int run_once_mode(
    const std::vector<std::shared_ptr<pulso::collectors::ICollector>>& collectors,
    const std::string& format)
{
    using pulso::utils::logging::Logger;
    auto& log = Logger::instancia();

    log.info("Modo once: ejecutando coleccion unica...");

    // 1. COLECCIONAR: ejecutar cada collector una sola vez
    std::vector<std::vector<pulso::core::Metrica>> all_metrics;
    for (const auto& collector : collectors)
    {
        auto metrics = collector->recolectar();
        all_metrics.push_back(std::move(metrics));
    }

    // 2. CONSTRUIR SNAPSHOT
    pulso::core::Snapshot snapshot;
    snapshot.timestamp = std::time(nullptr);
    for (const auto& metrics : all_metrics)
    {
        for (const auto& m : metrics)
        {
            snapshot.metricas.push_back(m);
        }
    }

    // 3. FORMATEAR con los formatters OFICIALES
std::unique_ptr<pulso::formatters::IFormatter> formatter;

if (format == "json")
{
    formatter = std::make_unique<pulso::formatters::FormatterJSON>();
}
else if (format == "csv")
{
    formatter = std::make_unique<pulso::formatters::FormatterCSV>();
}
else if (format == "prometheus")
{
    formatter = std::make_unique<pulso::formatters::FormatterPrometheus>();
}

// Validación explícita del formato recibido por CLI
if (!formatter)
{
    std::cerr << "[pulso] Formato no soportado: "
              << format
              << "\n";

    return 1;
}

std::string output = formatter->formatear(snapshot);

    // 4. IMPRIMIR a stdout
    std::cout << output;
    if (!output.empty() && output.back() != '\n')
    {
        std::cout << "\n";
    }

    log.info("Modo once: completado. Saliendo con codigo 0.");
    return 0;
}

// ============================================
// MAIN
// ============================================

int main(int argc, char* argv[]) {
    // -------------------------------------------------------------------------
    // 1. Parsear argumentos CLI
    // -------------------------------------------------------------------------
    pulso::cli::CliOptions cli_opts;
    if (!pulso::cli::parse_arguments(argc, argv, cli_opts)) {
        return 1;
    }

    // --config se maneja aqui (manteniendo logica original)
    std::string config_path = "pulso.toml";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            config_path = argv[++i];
        }
    }

    // -------------------------------------------------------------------------
    // 2. Cargar configuracion
    // -------------------------------------------------------------------------
    pulso::config::Config cfg;
    try {
        cfg = pulso::config::cargar(config_path);
    } catch (const pulso::config::ErrorConfig& e) {
        std::cerr << "[pulso] Error al cargar configuracion: " << e.what() << "\n";
        return 1;
    }

    // -------------------------------------------------------------------------
    // 3. Configurar logger
    // -------------------------------------------------------------------------
    using pulso::utils::logging::LogLevel;
    using pulso::utils::logging::Logger;

    auto& log = Logger::instancia();
    if      (cfg.nivel_log == "debug") log.setMinLevel(LogLevel::DEBUG);
    else if (cfg.nivel_log == "warn")  log.setMinLevel(LogLevel::WARN);
    else if (cfg.nivel_log == "error") log.setMinLevel(LogLevel::ERROR);
    else                               log.setMinLevel(LogLevel::INFO);

    log.info("pulso v" + pulso::APP_VERSION + " iniciando");

    // -------------------------------------------------------------------------
    // 4. Collectors (comun para modo once Y modo daemon)
    // -------------------------------------------------------------------------
    std::vector<std::shared_ptr<pulso::collectors::ICollector>> collectors;

    // Si no se pasa --metrics mantiene comportamiento actual:
    // todos los collectors activos por defecto
    bool usar_todos = cli_opts.monitor.cpu &&
                  cli_opts.monitor.ram &&
                  cli_opts.monitor.disk;

    if (cli_opts.monitor.cpu || usar_todos)
    {
       collectors.push_back(
          std::make_shared<pulso::collectors::CollectorCPU>()
       );
    }

    if (cli_opts.monitor.ram || usar_todos)
    {
       collectors.push_back(
          std::make_shared<pulso::collectors::memory::CollectorMemory>()
       );
    }

    // CollectorBateria se mantiene activo porque todavía no existe flag dedicado
     collectors.push_back(
        std::make_shared<pulso::collectors::bateria::CollectorBateria>()
     );

    // TODO: agregar CollectorDisk y CollectorNetwork cuando implementen ICollector.

    // =========================================================================
    // MODO ONCE
    // =========================================================================
    if (cli_opts.once)
    {
        return run_once_mode(collectors, cli_opts.format);
    }

    // =========================================================================
    // MODO DAEMON (comportamiento original - sin cambios)
    // =========================================================================

    log.info("Puerto: "        + std::to_string(cfg.servidor.puerto));
    log.info("Base de datos: " + cfg.storage.ruta_db);

    // Abrir base de datos e inicializar esquema
    pulso::storage::Storage storage(cfg.storage.ruta_db);
    pulso::storage::inicializarEsquema(storage);

    // Sampler (bucle infinito de coleccion)
    pulso::sampler::Sampler sampler(
        collectors,
        storage,
        cfg.sampler.intervalo_segundos
    );
    sampler.iniciar();

    // Signal handler
    setupSignalHandler();

    // Servidor HTTP + handlers
    httplib::Server server;
    pulso::formatters::FormatterJSON formatterJson;

    auto start_time = std::chrono::steady_clock::now();

    // GET /health
    server.Get("/health", [&start_time](
        const httplib::Request&,
        httplib::Response& res)
    {
        res.set_content(
            pulso::http::handleHealth(start_time),
            "application/json"
        );
    });

    // GET /version 
server.Get("/version", [](
    const httplib::Request&,
    httplib::Response& res) 
{
    res.set_content(
        pulso::http::handleVersion(),
        "application/json"
    );
});
server.Get("/config", handle_config);

    // GET /metrics — pendiente hasta que SystemMonitor se adapte al flujo
    // actual (Storage + ICollector). Ver issue #270.
    // pulso::http::HandleMetrics(server, system_monitor);

    // GET /history
    pulso::http::registrarHistory(server, storage, formatterJson);

    // GET /metrics/prometheus
    pulso::http::registrarPrometheus(server, storage);

    // Arrancar servidor HTTP en thread separado
    std::thread http_thread([&]() {
        log.info("Servidor HTTP escuchando en " +
                 cfg.servidor.host + ":" +
                 std::to_string(cfg.servidor.puerto));
        server.listen(cfg.servidor.host.c_str(), cfg.servidor.puerto);
    });

    // Esperar senal de shutdown (bucle infinito)
    while (isRunning.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Shutdown limpio
    log.info("Senal recibida — iniciando shutdown...");
    server.stop();
    if (http_thread.joinable()) http_thread.join();
    sampler.detener();
    log.info("pulso detenido correctamente.");

    return 0;
}
