
#include <cmath>
#include <stdexcept>
#include <string>

/**
 * Clasifica el nivel de utilizacion de un recurso
 * segun el porcentaje recibido.
 *
 * @param porcentaje Porcentaje de utilizacion entre 0 y 100.
 * @return Categoria: Bajo, Moderado o Alto.
 * @throws std::invalid_argument Si el porcentaje no es valido.
 */
std::string clasificar_utilizacion(double porcentaje) {
    if (!std::isfinite(porcentaje) ||
        porcentaje < 0.0 ||
        porcentaje > 100.0) {
        throw std::invalid_argument(
            "El porcentaje debe estar entre 0 y 100."
        );
    }

    if (porcentaje < 40.0) {
        return "Bajo";
    }

    if (porcentaje < 75.0) {
        return "Moderado";
    }

    return "Alto";
}
