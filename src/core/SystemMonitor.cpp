#include "SystemMonitor.hpp"

#include <chrono>
#include <iostream>
#include <map>
#include <string>

// ─── Declaraciones de funciones del flujo legacy ─────────────────────────────

/// @brief Obtiene el porcentaje de uso de CPU leyendo /proc/stat.
double ObtenerUsoCPU();

/// @brief Obtiene el porcentaje de uso de disco.
double getDiskUsage();

/// @brief Obtiene el porcentaje de uso de RAM.
double getRAMUsage();

/**
 * @brief Agrupa bytes de entrada y salida de la interfaz de red principal.
 */
struct NetworkIO {
    long rx; ///< Bytes recibidos
    long tx; ///< Bytes enviados
};

/// @brief Obtiene los contadores de red del sistema.
NetworkIO getNetworkIO();

// Implementación de SystemMonitor
/**
 * @brief Inicializa el monitor con estado detenido y lista de collectors vacía.
 */
SystemMonitor::SystemMonitor()
    : running_(false) {
}

// ─────────────────────────── API principal ───────────────────────────────────

/**
 * @brief Agrega un collector a la lista interna.
 *
 * El collector se almacena como shared_ptr. Se puede llamar antes o después
 * de start() sin efecto sobre el estado del monitor.
 *
 * @param collector  Implementación de ICollector a registrar. No debe ser
 *                   nullptr; de serlo, el comportamiento es indefinido.
 */
void SystemMonitor::addCollector(
    std::shared_ptr<chuspapulse::collectors::ICollector> collector)
{
    collectors_.push_back(std::move(collector));
}

/**
 * @brief Ejecuta todos los collectors y combina sus métricas en un Snapshot.
 *
 * Toma el timestamp Unix una sola vez al inicio del ciclo para que todas las
 * métricas compartan el mismo instante de captura. Por cada collector:
 *  - Invoca recolectar().
 *  - Inserta las Metrica devueltas al final del vector del snapshot.
 *  - Si el collector lanza std::exception, imprime el error en stderr y
 *    continúa con el siguiente (fail-safe).
 *
 * Un monitor sin collectors registrados retorna un Snapshot con timestamp
 * actual y vector de métricas vacío, sin lanzar ninguna excepción.
 *
 * @return Snapshot consolidado con métricas de todos los collectors activos.
 */
chuspapulse::core::Snapshot SystemMonitor::collectAll() {
    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = static_cast<std::int64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );

    for (const auto& collector : collectors_) {
        try {
            auto metricas = collector->recolectar();
            snapshot.metricas.insert(
                snapshot.metricas.end(),
                metricas.begin(),
                metricas.end()
            );
        } catch (const std::exception& e) {
            std::cerr << "[SystemMonitor] Error en collector \""
                      << collector->nombre()
                      << "\": " << e.what() << "\n";
        }
    }

    return snapshot;
}

// ─────────────────────────── API legacy ──────────────────────────────────────

/**
 * @brief Activa el monitor y ejecuta una captura con los collectors legacy.
 *
 * Mantiene el comportamiento original: llama a las funciones libres
 * ObtenerUsoCPU(), getRAMUsage(), getDiskUsage() y getNetworkIO() para
 * rellenar el mapa interno metrics_. Se conserva para compatibilidad con
 * handler_metrics.cpp hasta que ese módulo migre al flujo ICollector.
 *
 * @deprecated Preferir addCollector() + collectAll() para nuevas integraciones.
 */
void SystemMonitor::start() {
    running_ = true;

    const auto inicio = std::chrono::steady_clock::now();

    metrics_["cpu"]  = ObtenerUsoCPU();
    metrics_["ram"]  = getRAMUsage();
    metrics_["disk"] = getDiskUsage();

    const NetworkIO net = getNetworkIO();
    metrics_["net.rx"] = static_cast<double>(net.rx);
    metrics_["net.tx"] = static_cast<double>(net.tx);

    [[maybe_unused]] const auto duracion =
        std::chrono::steady_clock::now() - inicio;
}

/**
 * @brief Marca el monitor como detenido.
 *
 * @deprecated Parte de la API legacy; sin efecto sobre addCollector/collectAll.
 */
void SystemMonitor::stop() {
    running_ = false;
}

/**
 * @brief Retorna el mapa de métricas de la última llamada a start().
 *
 * @return Copia del mapa interno metrics_.
 *
 * @deprecated Preferir collectAll() que retorna un Snapshot tipado con
 *             todas las métricas de los collectors registrados.
 */
std::map<std::string, double> SystemMonitor::getMetrics() const {
    return metrics_;
}
