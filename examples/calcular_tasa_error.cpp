
#include <stdexcept>

/**
 * Calcula el porcentaje de operaciones fallidas.
 *
 * @param operaciones_totales Cantidad total de operaciones.
 * @param operaciones_fallidas Cantidad de operaciones fallidas.
 * @return Tasa de error expresada como porcentaje.
 * @throws std::invalid_argument Si los valores son invalidos.
 */
double calcular_tasa_error(
    int operaciones_totales,
    int operaciones_fallidas
) {
    if (operaciones_totales < 0 || operaciones_fallidas < 0) {
        throw std::invalid_argument(
            "Las cantidades no pueden ser negativas."
        );
    }

    if (operaciones_totales == 0) {
        throw std::invalid_argument(
            "El total de operaciones debe ser mayor que cero."
        );
    }

    if (operaciones_fallidas > operaciones_totales) {
        throw std::invalid_argument(
            "Las operaciones fallidas no pueden superar el total."
        );
    }

    return (static_cast<double>(operaciones_fallidas) /
            operaciones_totales) * 100.0;
}
