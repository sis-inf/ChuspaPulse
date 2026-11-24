
#include <cstdint>
#include <iostream>
#include <string>

/**
 * Identifica el orden de bytes utilizado por el sistema.
 *
 * @return Una cadena indicando Little Endian, Big Endian
 *         o Desconocida.
 */
std::string obtener_endianness() {
    const std::uint32_t valor = 1;

    const unsigned char* bytes =
        reinterpret_cast<const unsigned char*>(&valor);

    if (bytes[0] == 1) {
        return "Little Endian";
    }

    if (bytes[0] == 0 &&
        bytes[sizeof(valor) - 1] == 1) {
        return "Big Endian";
    }

    return "Desconocida";
}

int main() {
    std::cout << "Detector de Endianness" << std::endl;
    std::cout << "----------------------" << std::endl;

    std::string resultado = obtener_endianness();

    std::cout << "Orden de bytes del sistema: "
              << resultado << std::endl;

    return 0;
}
