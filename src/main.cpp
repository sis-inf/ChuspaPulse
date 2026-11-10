#include "http/handler_alerts.hpp"
#include "alertas/gestor_alertas.hpp"
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
#include "collectors/disk/disk_usage.h"
#include "collectors/network/net_usage.h"
#include "platform/linux/collector_cpu.hpp"
#include "collectors/procesos/proc_collector.h"
#include "collectors/loadavg/loadavg_collector.h"
#include "collectors/uptime/uptime_collector.h"
#include "collectors/temperatura/temp_collector.h"

// Sampler
#include "sampler/sampler.hpp"

// Formatters (OFICIALES del proyecto)
#include "formatters/formatter_json.hpp"
#include "formatters/formatter_csv.hpp"
#include "formatters/formatter_prometheus.hpp"
#include "formatters/formatter_text.hpp"

// HTTP handlers
#include "http/handler_config.hpp"
#include "http/handler_health.hpp"
#include "http/handler_history.hpp"
#include "http/handler_alerts_history.hpp"
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
bool consume_reload_request();
class CollectorProcesos : public pulso::collectors::ICollector {
public:
    std::string nombre() const override {
        return "procesos";
    }

    std::vector<pulso::core::Metrica> recolectar() override {
        pulso::collectors::ProcInfo info = pulso::collectors::getProcInfo();
        auto ts = std::time(nullptr);

        return {
            {"procesos.running", static_cast<double>(info.running), "cantidad", ts},
            {"procesos.blocked", static_cast<double>(info.blocked), "cantidad", ts},
            {"procesos.total", static_cast<double>(info.total), "cantidad", ts}
        };
    }
};

class CollectorLoadAvg : public pulso::collectors::ICollector {
public:
    std::string nombre() const override {
        return "loadavg";
    }

    std::vector<pulso::core::Metrica> recolectar() override {
        LoadAvgInfo info = getLoadAverage();
        auto ts = std::time(nullptr);

        return {
            {"loadavg.1", static_cast<double>(info.load1), "carga", ts},
            {"loadavg.5", static_cast<double>(info.load5), "carga", ts},
            {"loadavg.15", static_cast<double>(info.load15), "carga", ts}
        };
    }
};

class CollectorUptime : public pulso::collectors::ICollector {
public:
    std::string nombre() const override {
        return "uptime";
    }

    std::vector<pulso::core::Metrica> recolectar() override {
        UptimeInfo info = getUptime();
        auto ts = std::time(nullptr);

        return {
            {"uptime.seconds", static_cast<double>(info.total_seconds), "segundos", ts}
        };
    }
};

class CollectorTemperatura : public pulso::collectors::ICollector {
public:
    std::string nombre() const override {
        return "temperatura";
    }

    std::vector<pulso::core::Metrica> recolectar() override {
        ::collectors::TempInfo info = ::collectors::getTempInfo();

        if (!info.disponible) {
            return {};
        }

        auto ts = std::time(nullptr);

        return {
            {"temperatura.cpu", static_cast<double>(info.cpu_celsius), "celsius", ts}
        };
    }
};

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
else if (format == "text")
{
    formatter = std::make_unique<pulso::formatters::FormatterText>();
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

/**
 * @brief Convierte el Snapshot del sampler (lista de métricas) al
 *        MetricSnapshot que evalúa GestorAlertas.
 *
 * CPU llega en porcentaje. Disco y RAM llegan en bytes, así que se
 * convierten a porcentaje para poder compararlos con los umbrales.
 */
static pulso::MetricSnapshot construirMetricSnapshot(
    const pulso::core::Snapshot& snap)
{
    pulso::MetricSnapshot ms;

    double discoUsado = 0.0;
    double discoTotal = 0.0;

    for (const auto& m : snap.metricas) {
        if (m.nombre == "cpu.usage") {
            ms.cpu = m.valor;
        } else if (m.nombre == "disk.used") {
            discoUsado = m.valor;
        } else if (m.nombre == "disk.total") {
            discoTotal = m.valor;
        }
    }

    if (discoTotal > 0.0) {
        ms.disk = discoUsado / discoTotal * 100.0;
    }

    // La métrica "memory.used" no trae el total, así que se consulta aparte.
    try {
        const auto ram = pulso::collectors::memory::getRamUsage();
        if (ram.total > 0) {
            ms.ram = static_cast<double>(ram.used) /
                     static_cast<double>(ram.total) * 100.0;
        }
    } catch (const std::exception&) {
        // Si no se puede leer la RAM, queda en 0 y no dispara alerta.
    }

    return ms;
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

    // --config ya fue validado por parse_arguments
    std::string config_path = cli_opts.config_path;

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

    if (cli_opts.monitor.disk || usar_todos)
    {
        collectors.push_back(
            std::make_shared<pulso::collectors::disk::DiskCollector>()
        );
    }

    // La CLI todavía no tiene un flag dedicado para red, por eso se mantiene activa.
    collectors.push_back(
        std::make_shared<pulso::collectors::network::NetworkCollector>()
    );
    collectors.push_back(
        std::make_shared<CollectorProcesos>()
    );

    collectors.push_back(
        std::make_shared<CollectorLoadAvg>()
    );

    collectors.push_back(
        std::make_shared<CollectorUptime>()
    );

    collectors.push_back(
        std::make_shared<CollectorTemperatura>()
    );


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


    // Gestor de alertas con umbrales leídos desde pulso.toml
        // Conexión propia a la misma base de datos para guardar y consultar alertas
    SQLite::Database alertasDb(
        cfg.storage.ruta_db,
        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE,
        5000);  // espera hasta 5 s si el sampler está escribiendo
    pulso::storage::inicializarEsquema(alertasDb);
    pulso::alertas::GestorAlertas gestorAlertas(alertasDb);
    gestorAlertas.addAlerta(pulso::alertas::AlertaUmbral(
        "cpu", static_cast<float>(cfg.umbrales.cpu_max), '>'));
    gestorAlertas.addAlerta(pulso::alertas::AlertaUmbral(
        "ram", static_cast<float>(cfg.umbrales.ram_max), '>'));
    gestorAlertas.addAlerta(pulso::alertas::AlertaUmbral(
        "disk", static_cast<float>(cfg.umbrales.disco_max), '>'));

    // Cuando una alerta se dispara, queda registrada en el log
    gestorAlertas.setCallback([](const std::string& mensaje) {
        pulso::utils::logging::Logger::instancia().warn(mensaje);
    });

    log.info("GestorAlertas activo -> Umbrales: CPU=" +
             std::to_string(cfg.umbrales.cpu_max) + "%, RAM=" +
             std::to_string(cfg.umbrales.ram_max) + "%, Disco=" +
             std::to_string(cfg.umbrales.disco_max) + "%");

    // Sampler (bucle de colección); en cada ciclo evalúa las alertas
    pulso::sampler::Sampler sampler(
        collectors,
        storage,
        cfg.sampler.intervalo_segundos,
        [&gestorAlertas](const pulso::core::Snapshot& snap) {
            gestorAlertas.evaluar(construirMetricSnapshot(snap));
        }
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

    // GET /alerts
    server.Get("/alerts", [&storage, &gestorAlertas](
        const httplib::Request&,
        httplib::Response& res)
    {
        pulso::MetricSnapshot snap;  // en ceros si todavía no hay datos
        if (auto ultimo = storage.last()) {
            snap = construirMetricSnapshot(*ultimo);
        }

        res.status = 200;
        res.set_content(
            pulso::http::handleAlerts(gestorAlertas, snap),
            "application/json"
        );
    });


    // GET /metrics — último snapshot de Storage en formato Prometheus
    pulso::http::HandleMetrics(server, storage);

    // GET /history
    pulso::http::registrarHistory(server, storage, formatterJson);

    // GET /metrics/prometheus
    pulso::http::registrarPrometheus(server, storage);
    // GET /alerts/history
    pulso::http::registrarAlertsHistory(server, gestorAlertas);


    // Arrancar servidor HTTP en thread separado.
    // Host y puerto se copian porque cfg puede reemplazarse al recibir SIGHUP.
    const std::string http_host = cfg.servidor.host;
    const int http_puerto = cfg.servidor.puerto;
    std::thread http_thread([&, http_host, http_puerto]() {
        log.info("Servidor HTTP escuchando en " +
                 http_host + ":" +
                 std::to_string(http_puerto));
        server.listen(http_host.c_str(), http_puerto);
    });

    // Esperar senal de shutdown (bucle infinito)
    while (isRunning.load()) {
        // SIGHUP: releer y validar la configuracion sin detener el proceso.
        // Si es valida pasa a ser la configuracion activa; el servidor, la
        // base de datos, el sampler, el logger y los umbrales siguen con los
        // valores del arranque hasta reiniciar. Si es invalida se conserva
        // la activa.
        if (consume_reload_request()) {
            log.info("SIGHUP recibido: recargando configuracion desde " + config_path);

            pulso::config::Config recargada = cfg;
            std::string error;

            if (pulso::config::recargar(config_path, recargada, error)) {
                const auto campos = pulso::config::camposModificados(cfg, recargada);

                if (campos.empty()) {
                    log.info("Configuracion recargada y valida: sin cambios.");
                } else {
                    std::string lista;
                    for (const auto& campo : campos) {
                        if (!lista.empty()) lista += ", ";
                        lista += campo;
                    }
                    log.warn("Configuracion recargada y valida. Campos modificados "
                             "que requieren reiniciar el agente: " + lista);
                }

                cfg = recargada;
            } else {
                log.error("No se pudo recargar la configuracion: " + error +
                          ". Se conserva la configuracion activa.");
            }
        }

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
