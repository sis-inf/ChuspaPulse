
#include <cmath>
#include <stdexcept>

/**
 * Calcula la variacion porcentual entre dos mediciones.
 *
 * Formula:
 * variacion = ((valor_actual - valor_anterior) / valor_anterior) * 100
 *
 * @param valor_anterior Valor de la medicion anterior.
 * @param valor_actual Valor de la medicion actual.
 * @return Variacion porcentual.
 *
 * @throws std::invalid_argument Si los valores no son finitos
 *         o alguno es negativo.
 * @throws std::domain_error Si el valor anterior es cero.
 * @throws std::overflow_error Si el resultado no es finito.
 */
double calcular_variacion_porcentual(
    double valor_anterior,
    double valor_actual
) {
    if (!std::isfinite(valor_anterior) ||
        !std::isfinite(valor_actual)) {
        throw std::invalid_argument(
            "Los valores deben ser numericos y finitos."
        );
    }

    if (valor_anterior < 0 || valor_actual < 0) {
        throw std::invalid_argument(
            "No se permiten valores negativos."
        );
    }

    if (valor_anterior == 0) {
        throw std::domain_error(
            "El valor anterior no puede ser cero."
        );
    }

    const double variacion =
        ((valor_actual - valor_anterior) / valor_anterior) * 100.0;

    if (!std::isfinite(variacion)) {
        throw std::overflow_error(
            "El resultado de la variacion porcentual no es finito."
        );
    }

    return variacion;
}
