
#include <iostream>
#include <string>
#include <cstdint>
#include <iomanip>

std::uint32_t calcular_checksum(const std::string& texto) {
    std::uint32_t hash = 2166136261u;
    const std::uint32_t primo = 16777619u;

    for (unsigned char caracter : texto) {
        hash ^= caracter;
        hash *= primo;
    }

    return hash;
}

int main() {
    std::string texto;

    std::cout << "Ingrese un texto: ";
    std::getline(std::cin, texto);

    std::uint32_t resultado = calcular_checksum(texto);

    std::cout << "Checksum FNV-1a: "
              << std::hex << std::uppercase
              << std::setw(8) << std::setfill('0')
              << resultado << std::endl;

    return 0;
}
