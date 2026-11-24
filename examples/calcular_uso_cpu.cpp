
#include <cmath>
#include <stdexcept>

/**
 * Calcula el porcentaje de utilizacion de CPU entre dos muestras.
 *
 * @param total_anterior Tiempo total de la primera muestra.
 * @param inactivo_anterior Tiempo inactivo de la primera muestra.
 * @param total_actual Tiempo total de la segunda muestra.
 * @param inactivo_actual Tiempo inactivo de la segunda muestra.
 * @return Porcentaje de utilizacion de CPU entre 0 y 100.
 * @throws std::invalid_argument Si los valores son inconsistentes
 *         o no existe diferencia de tiempo entre las muestras.
 */
double calcular_uso_cpu(
    double total_anterior,
    double inactivo_anterior,
    double total_actual,
    double inactivo_actual
) {
    if (!std::isfinite(total_anterior) ||
        !std::isfinite(inactivo_anterior) ||
        !std::isfinite(total_actual) ||
        !std::isfinite(inactivo_actual)) {
        throw std::invalid_argument("Los valores deben ser finitos.");
    }

    if (total_anterior < 0 || inactivo_anterior < 0 ||
        total_actual < 0 || inactivo_actual < 0) {
        throw std::invalid_argument("Los valores no pueden ser negativos.");
    }

    if (inactivo_anterior > total_anterior ||
        inactivo_actual > total_actual) {
        throw std::invalid_argument("Los tiempos inactivos no pueden superar los totales.");
    }

    if (total_actual <= total_anterior ||
        inactivo_actual < inactivo_anterior) {
        throw std::invalid_argument("Las muestras no son consistentes.");
    }

    double diferencia_total = total_actual - total_anterior;
    double diferencia_inactiva = inactivo_actual - inactivo_anterior;

    if (diferencia_total <= 0) {
        throw std::invalid_argument("No se puede dividir entre cero.");
    }

    if (diferencia_inactiva > diferencia_total) {
        throw std::invalid_argument("La diferencia inactiva supera el tiempo total.");
    }

    double tiempo_activo = diferencia_total - diferencia_inactiva;

    return (tiempo_activo / diferencia_total) * 100.0;
}
