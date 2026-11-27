#pragma once

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "../icollector.hpp"
#include "core/types.hpp"

namespace chuspapulse::collectors::bateria {

/**
 * @brief Contiene la informacion de estado de bateria del sistema.
 */
struct BateriaInfo {
    /** @brief Porcentaje de carga actual de la bateria (0-100). */
    int porcentaje = 0;

    /** @brief Estado de carga reportado por el kernel (p. ej. "Charging",
     *         "Discharging", "Full", "Not charging", "Unknown"). */
    std::string estado{};

    /** @brief Indica si se encontro informacion de bateria en el sistema.
     *         false en servidores o equipos sin bateria (BAT0 inexistente). */
    bool disponible = false;
    bool esValida() const noexcept {
        return disponible && porcentaje >= 0 && porcentaje <= 100 && !estado.empty();
    }
};

/**
 * @brief Lee el estado de la bateria desde /sys/class/power_supply/BAT0/.
 *
 * Lee el porcentaje de carga desde el archivo "capacity" y el estado de
 * carga desde el archivo "status". Si el directorio o los archivos no
 * existen (caso comun en servidores sin bateria), retorna BateriaInfo
 * con disponible=false, sin lanzar excepcion.
 *
 * @param base_path Ruta base del dispositivo de bateria (por defecto
 *                   "/sys/class/power_supply/BAT0"). Permite inyectar una
 *                   ruta alternativa en tests sin modificar el codigo de
 *                   produccion.
 * @return BateriaInfo con los valores leidos, o disponible=false si no
 *         hay bateria presente.
 */
BateriaInfo getBateriaInfo(
    const std::string& base_path = "/sys/class/power_supply/BAT0"
);

inline BateriaInfo getBateriaInfoSeguro(
    const std::string& base_path = "/sys/class/power_supply/BAT0"
) noexcept {
    try {
        if (base_path.empty()) {
            std::cerr << "[bateria_collector] Ruta base vacia al consultar informacion de bateria.\n";
            return BateriaInfo{};
        }
        return getBateriaInfo(base_path);
    } catch (const std::exception& ex) {
        try {
            std::cerr << "[bateria_collector] Fallo controlado al leer bateria en '"
                      << base_path << "': " << ex.what() << "\n";
        } catch (...) {
            std::cerr << "[bateria_collector] Fallo controlado al leer bateria.\n";
        }
        return BateriaInfo{};
    } catch (...) {
        std::cerr << "[bateria_collector] Error desconocido controlado al leer bateria.\n";
        return BateriaInfo{};
    }
}

/**
 * @brief Collector de metricas de bateria.
 *
 * Implementa la interfaz ICollector. En hosts sin bateria (la mayoria de
 * servidores, que es el caso mas comun para este proyecto) recolectar()
 * retorna un vector vacio, sin lanzar excepcion ni agregar metricas
 * vacias o enganosas.
 */
class CollectorBateria : public chuspapulse::collectors::ICollector {
public:
    std::string nombre() const override;

    std::vector<chuspapulse::core::Metrica> recolectar() override;
    std::vector<chuspapulse::core::Metrica> recolectarSinAbortar() noexcept {
        try {
            return recolectar();
        } catch (const std::exception& ex) {
            try {
                std::cerr << "[bateria_collector] Fallo controlado durante recoleccion: "
                          << ex.what() << "\n";
            } catch (...) {
                std::cerr << "[bateria_collector] Fallo controlado durante recoleccion.\n";
            }
            return {};
        } catch (...) {
            std::cerr << "[bateria_collector] Error desconocido controlado durante recoleccion.\n";
            return {};
        }
    }
};

} // namespace chuspapulse::collectors::bateria
