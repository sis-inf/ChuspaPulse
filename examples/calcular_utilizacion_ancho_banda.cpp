
#include <iostream>

double calcular_utilizacion_ancho_banda(double velocidad_actual,
                                        double capacidad_total) {
    if (velocidad_actual < 0 || capacidad_total <= 0) {
        return -1.0;
    }

    if (velocidad_actual > capacidad_total) {
        return -1.0;
    }

    return (velocidad_actual / capacidad_total) * 100.0;
}

int main() {
    double velocidad_actual;
    double capacidad_total;

    std::cout << "Calculadora de utilizacion del ancho de banda"
              << std::endl;

    std::cout << "Ingrese la velocidad actual (bps): ";
    std::cin >> velocidad_actual;

    std::cout << "Ingrese la capacidad total (bps): ";
    std::cin >> capacidad_total;

    double resultado = calcular_utilizacion_ancho_banda(
        velocidad_actual, capacidad_total
    );

    if (resultado < 0) {
        std::cout << "Error: valores no validos." << std::endl;
    } else {
        std::cout << "Utilizacion del ancho de banda: "
                  << resultado << "%" << std::endl;
    }

    return 0;
}
