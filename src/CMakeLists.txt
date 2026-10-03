add_executable(pulso main.cpp)

target_sources(pulso PRIVATE
    cli/arg_parser.cpp
    collectors/bateria/bateria_collector.cpp
    collectors/cpu/cpu_usage.cpp
    collectors/disk/disk_usage.cpp
    collectors/memory/ram_usage.cpp
    collectors/network/network_io.cpp
    collectors/network/net_usage.cpp
    collectors/loadavg/loadavg_collector.cpp
    collectors/procesos/proc_collector.cpp
    collectors/procesos/top_procesos.cpp
    collectors/temperatura/temp_collector.cpp
    config/config.cpp
    core/SystemMonitor.cpp
    formatters/formatter_json.cpp
    formatters/formatter_csv.cpp
    formatters/formatter_text.cpp
    formatters/formatter_prometheus.cpp
    http/handler_health.cpp
    http/handler_history.cpp
    http/handler_metrics.cpp
    platform/linux/collector_cpu.cpp
    storage/schema.cpp
    storage/storage.cpp
    utils/console_formatter.cpp
    utils/formato_bytes.cpp
    utils/logging/logger.cpp
    utils/signal_handler.cpp
    src/core/version.hpp
    src/http/handler_version.hpp
    src/http/handler_version.cpp
    alertas/alerta_umbral.cpp
    alertas/gestor_alertas.cpp
)

# Collector de CPU para Windows: solo se compila en plataforma WIN32.
# En Linux/macOS este archivo queda completamente excluido del build.
if(WIN32)
    target_sources(pulso PRIVATE
        platform/windows/collector_cpu.cpp
    )
endif()

target_compile_features(pulso PRIVATE cxx_std_17)

target_include_directories(pulso PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pulso PRIVATE
    httplib
    nlohmann_json
    tomlplusplus
    SQLiteCpp
    sqlite3
)

target_compile_options(pulso PRIVATE -Wall -Wextra -Wpedantic -O2)
