#pragma once

#include <cstdint>
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
    int porcentaje;

    /** @brief Estado de carga reportado por el kernel (p. ej. "Charging",
     *         "Discharging", "Full", "Not charging", "Unknown"). */
    std::string estado;

    /** @brief Indica si se encontro informacion de bateria en el sistema.
     *         false en servidores o equipos sin bateria (BAT0 inexistente). */
    bool disponible;
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
};

} // namespace chuspapulse::collectors::bateria
