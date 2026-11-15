#pragma once

#ifdef _WIN32

#include "../../collectors/icollector.hpp"

#include <string>
#include <vector>

namespace chuspapulse::platform::windows_platform {

/**
 * @brief Collector de uso de CPU para plataforma Windows.
 *
 * Implementa chuspapulse::collectors::ICollector usando la API de Windows
 * GetSystemTimes() para calcular el porcentaje de uso de CPU con dos
 * lecturas separadas por un breve intervalo, replicando la semantica
 * de CollectorCPU (Linux).
 *
 * Expone las metricas:
 *   - cpu.usage  (porcentaje, 0-100)
 *   - cpu.cores  (cantidad de cores logicos, via GetSystemInfo)
 *
 * El nombre del collector es "cpu" para mantener compatibilidad de
 * metricas entre plataformas (igual que la version Linux).
 *
 * Solo se compila en Windows (_WIN32 definido). En Linux/macOS este
 * archivo queda completamente excluido de la compilacion.
 */
class CollectorCPUWindows : public chuspapulse::collectors::ICollector {
public:
    /**
     * @brief Retorna el nombre identificador del collector.
     * @return "cpu" — identico al collector Linux para compatibilidad
     *         de metricas entre plataformas.
     */
    std::string nombre() const override;

    /**
     * @brief Calcula el uso de CPU llamando a GetSystemTimes dos veces
     *        con un breve intervalo (120 ms) entre lecturas.
     *
     * @return Vector con las metricas cpu.usage y cpu.cores.
     * @throws chuspapulse::collectors::ErrorRecoleccion si GetSystemTimes
     *         falla (no deberia ocurrir en condiciones normales).
     */
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

} // namespace chuspapulse::platform::windows_platform

#endif // _WIN32