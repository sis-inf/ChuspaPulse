
#include <iostream>
#include <iomanip>

/**
 * Calcula la relacion entre bytes leidos y bytes escritos.
 *
 * @param bytes_leidos Cantidad de bytes leidos.
 * @param bytes_escritos Cantidad de bytes escritos.
 * @return Relacion calculada o -1.0 si los datos son invalidos.
 */
double calcular_relacion_lectura_escritura(
    double bytes_leidos,
    double bytes_escritos
) {
    // Validar que los valores no sean negativos.
    if (bytes_leidos < 0 || bytes_escritos < 0) {
        return -1.0;
    }

    // Evitar division entre cero.
    if (bytes_escritos == 0) {
        return -1.0;
    }

    // Calcular la relacion entre lecturas y escrituras.
    return bytes_leidos / bytes_escritos;
}

int main() {
    double bytes_leidos;
    double bytes_escritos;

    std::cout << "Calculadora de relacion de lectura y escritura"
              << std::endl;

    std::cout << "Ingrese los bytes leidos: ";
    std::cin >> bytes_leidos;

    std::cout << "Ingrese los bytes escritos: ";
    std::cin >> bytes_escritos;

    double resultado = calcular_relacion_lectura_escritura(
        bytes_leidos, bytes_escritos
    );

    if (resultado < 0) {
        std::cout << "Error: los valores ingresados no son validos."
                  << std::endl;
    } else {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Relacion lectura/escritura: "
                  << resultado << std::endl;
    }

    return 0;
}
