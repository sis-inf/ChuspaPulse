
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

/**
 * Calcula la mediana de un conjunto de mediciones numericas.
 *
 * Para una cantidad impar de elementos, devuelve el valor central.
 * Para una cantidad par, devuelve el promedio de los dos valores centrales.
 *
 * @param datos Vector de valores numericos.
 * @return Mediana calculada.
 * @throws std::invalid_argument Si el vector esta vacio o contiene
 *         valores no finitos.
 */
double calcular_mediana(const std::vector<double>& datos) {
    if (datos.empty()) {
        throw std::invalid_argument(
            "La coleccion de datos no puede estar vacia."
        );
    }

    std::vector<double> valores_ordenados = datos;

    for (double valor : valores_ordenados) {
        if (!std::isfinite(valor)) {
            throw std::invalid_argument(
                "Todos los valores deben ser numericos y finitos."
            );
        }
    }

    std::sort(valores_ordenados.begin(), valores_ordenados.end());

    const std::size_t cantidad = valores_ordenados.size();
    const std::size_t centro = cantidad / 2;

    if (cantidad % 2 != 0) {
        return valores_ordenados[centro];
    }

    return valores_ordenados[centro - 1] / 2.0
         + valores_ordenados[centro] / 2.0;
}
