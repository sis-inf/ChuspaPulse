
#include <algorithm>
#include <stdexcept>
#include <vector>

/**
 * Calcula el rango de un conjunto de mediciones numericas.
 *
 * El rango corresponde a la diferencia entre el valor maximo
 * y el valor minimo del conjunto.
 *
 * @param datos Vector de valores numericos.
 * @return Diferencia entre el valor maximo y el minimo.
 * @throws std::invalid_argument Si la coleccion esta vacia.
 */
double calcular_rango(const std::vector<double>& datos) {
    if (datos.empty()) {
        throw std::invalid_argument(
            "La coleccion de datos no puede estar vacia."
        );
    }

    const auto extremos = std::minmax_element(
        datos.begin(),
        datos.end()
    );

    return *extremos.second - *extremos.first;
}
