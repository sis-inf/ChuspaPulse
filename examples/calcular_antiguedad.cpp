
#include <iostream>
#include <stdexcept>

long long calcular_antiguedad(long long timestamp_inicial,
                              long long timestamp_final) {
    if (timestamp_inicial < 0 || timestamp_final < 0) {
        throw std::invalid_argument("Las marcas temporales no pueden ser negativas.");
    }

    if (timestamp_final < timestamp_inicial) {
        throw std::invalid_argument("La marca temporal final no puede ser anterior a la inicial.");
    }

    return timestamp_final - timestamp_inicial;
}

int main() {
    long long timestamp_inicial = 1700000000;
    long long timestamp_final = 1700003600;

    try {
        long long antiguedad = calcular_antiguedad(
            timestamp_inicial,
            timestamp_final
        );

        std::cout << "Tiempo transcurrido: "
                  << antiguedad << " segundos" << std::endl;
    } catch (const std::invalid_argument& error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
