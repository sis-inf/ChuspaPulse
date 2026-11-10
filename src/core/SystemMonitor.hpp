#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "collectors/icollector.hpp"
#include "core/types.hpp"

/**
 * @brief Orquestador de métricas del sistema basado en collectors dinámicos.
 *
 * Mantiene una lista interna de collectors registrados vía addCollector().
 * Al llamar a collectAll(), itera sobre ellos, combina todas sus métricas
 * en un único chuspapulse::core::Snapshot y lo retorna.
 *
 * Los errores de un collector individual se capturan y registran en stderr
 * sin interrumpir los demás (fail-safe).
 *
 * @note La API legacy start() / stop() / getMetrics() se conserva para
 *       compatibilidad con handler_metrics.cpp hasta que ese módulo
 *       migre al flujo Sampler + Storage.
 */
class SystemMonitor {

public:

    // ─────────────────────────── ciclo de vida ───────────────────────────────

    /**
     * @brief Constructor por defecto.
     *
     * Inicializa el monitor en estado detenido y con la lista de collectors
     * vacía. Es seguro llamar a addCollector() antes o después de start().
     */
    SystemMonitor();

    // ─────────────────────────── API principal ───────────────────────────────

    /**
     * @brief Registra un collector en la lista interna del monitor.
     *
     * El collector es almacenado como shared_ptr, por lo que el monitor
     * comparte la propiedad. Se puede llamar en cualquier momento, incluso
     * con el monitor en ejecución.
     *
     * @param collector  Puntero compartido a cualquier implementación de
     *                   chuspapulse::collectors::ICollector. No debe ser nullptr.
     *
     * @pre  `collector != nullptr`
     * @post El collector queda registrado y será invocado en el próximo
     *       collectAll().
     *
     * @see chuspapulse::collectors::ICollector
     * @see collectAll()
     */
    void addCollector(std::shared_ptr<chuspapulse::collectors::ICollector> collector);

    /**
     * @brief Ejecuta todos los collectors registrados y combina sus métricas.
     *
     * Itera sobre la lista interna de collectors en orden de registro.
     * Por cada collector:
     *  - Llama a ICollector::recolectar().
     *  - Añade las métricas devueltas al vector del snapshot.
     *  - Si el collector lanza una excepción, la captura, la registra en
     *    stderr y continúa con el siguiente (comportamiento fail-safe).
     *
     * Si no hay collectors registrados, retorna un snapshot vacío con el
     * timestamp actual y un vector de métricas vacío (sin crash).
     *
     * @return chuspapulse::core::Snapshot con todas las métricas combinadas y el
     *         timestamp Unix del momento de la llamada.
     *
     * @note El timestamp se toma una sola vez al inicio de la iteración para
     *       que todas las métricas del snapshot compartan el mismo instante.
     *
     * @see addCollector()
     * @see chuspapulse::core::Snapshot
     */
    chuspapulse::core::Snapshot collectAll();

    // ─────────────────────────── API legacy ──────────────────────────────────

    /**
     * @brief Marca el monitor como activo y ejecuta una captura inicial.
     *
     * Invoca los collectors hardcodeados legacy (CPU, RAM, disco, red)
     * y rellena el mapa interno `metrics`. Se conserva para compatibilidad
     * con handler_metrics.cpp.
     *
     * @deprecated Preferir addCollector() + collectAll() para nuevas
     *             integraciones.
     */
    void start();

    /**
     * @brief Marca el monitor como detenido.
     *
     * @deprecated Parte de la API legacy. Sin efecto en el flujo de
     *             addCollector() / collectAll().
     */
    void stop();

    /**
     * @brief Retorna el mapa de métricas de la última captura legacy.
     *
     * @return Mapa nombre → valor de la última llamada a start().
     *
     * @deprecated Preferir collectAll() que retorna un Snapshot tipado.
     */
    std::map<std::string, double> getMetrics() const;

private:

    /** @brief Indica si el monitor está en estado activo (API legacy). */
    bool running_;

    /**
     * @brief Lista de collectors registrados dinámicamente.
     *
     * Se itera en orden de registro dentro de collectAll().
     * Protegida por convención; no se usa mutex ya que SystemMonitor
     * no está diseñado para acceso concurrente en esta versión.
     */
    std::vector<std::shared_ptr<chuspapulse::collectors::ICollector>> collectors_;

    /** @brief Mapa de métricas de la API legacy (usado por getMetrics()). */
    std::map<std::string, double> metrics_;
};
