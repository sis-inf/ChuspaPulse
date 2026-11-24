
#include <iostream>
#include <iomanip>

/**
 * Calcula el porcentaje de aciertos de una cache.
 *
 * @param consultas_totales Cantidad total de consultas.
 * @param aciertos Cantidad de consultas atendidas desde la cache.
 * @return Porcentaje de aciertos o -1.0 si los datos son invalidos.
 */
double calcular_tasa_aciertos(int consultas_totales, int aciertos) {
    // Validar valores negativos.
    if (consultas_totales < 0 || aciertos < 0) {
        return -1.0;
    }

    // Evitar division entre cero.
    if (consultas_totales == 0) {
        return -1.0;
    }

    // Los aciertos no pueden superar las consultas totales.
    if (aciertos > consultas_totales) {
        return -1.0;
    }

    // Calcular el porcentaje de aciertos.
    return (static_cast<double>(aciertos) / consultas_totales) * 100.0;
}

int main() {
    int consultas_totales;
    int aciertos;

    std::cout << "Calculadora de tasa de aciertos de cache" << std::endl;

    std::cout << "Ingrese las consultas totales: ";
    std::cin >> consultas_totales;

    std::cout << "Ingrese la cantidad de aciertos: ";
    std::cin >> aciertos;

    double resultado = calcular_tasa_aciertos(
        consultas_totales, aciertos
    );

    if (resultado < 0) {
        std::cout << "Error: los valores ingresados no son validos."
                  << std::endl;
    } else {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Tasa de aciertos: "
                  << resultado << "%" << std::endl;
    }

    return 0;
}
