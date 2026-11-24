
#include <iostream>
#include <thread>

/**
 * Obtiene la cantidad de procesadores logicos disponibles.
 *
 * @return Cantidad de procesadores logicos.
 *         Devuelve 0 si no se puede determinar.
 */
unsigned int obtener_cantidad_procesadores() {
    unsigned int cantidad = std::thread::hardware_concurrency();

    if (cantidad > 0) {
        return cantidad;
    }

    return 0;
}

int main() {
    std::cout << "Detector de procesadores logicos" << std::endl;
    std::cout << "---------------------------------" << std::endl;

    unsigned int cantidad = obtener_cantidad_procesadores();

    if (cantidad > 0) {
        std::cout << "Procesadores logicos disponibles: "
                  << cantidad << std::endl;
    } else {
        std::cout << "No se pudo determinar la cantidad de procesadores."
                  << std::endl;
    }

    return 0;
}
