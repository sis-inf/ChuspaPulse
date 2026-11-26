
#include <iostream>
#include <string>
#include <vector>
#include <charconv>
#include <system_error>
#include <stdexcept>

std::vector<unsigned long long> separar_version(
    const std::string& version) {

    if (version.empty()) {
        throw std::invalid_argument("La version no puede estar vacia.");
    }

    std::vector<unsigned long long> componentes;
    std::string::size_type inicio = 0;

    while (inicio < version.size()) {
        std::string::size_type fin = version.find('.', inicio);

        if (fin == std::string::npos) {
            fin = version.size();
        }

        if (fin == inicio) {
            throw std::invalid_argument("Componente vacio.");
        }

        unsigned long long numero = 0;

        const char* primero = version.data() + inicio;
        const char* ultimo = version.data() + fin;

        auto resultado = std::from_chars(primero, ultimo, numero);

        if (resultado.ec != std::errc() ||
            resultado.ptr != ultimo) {
            throw std::invalid_argument(
                "La version contiene componentes invalidos.");
        }

        componentes.push_back(numero);
        inicio = fin + 1;
    }

    return componentes;
}

int comparar_versiones(
    const std::string& version_a,
    const std::string& version_b) {

    std::vector<unsigned long long> a =
        separar_version(version_a);

    std::vector<unsigned long long> b =
        separar_version(version_b);

    std::size_t cantidad =
        a.size() > b.size() ? a.size() : b.size();

    for (std::size_t i = 0; i < cantidad; ++i) {
        unsigned long long valor_a =
            i < a.size() ? a[i] : 0;

        unsigned long long valor_b =
            i < b.size() ? b[i] : 0;

        if (valor_a < valor_b) {
            return -1;
        }

        if (valor_a > valor_b) {
            return 1;
        }
    }

    return 0;
}

int main() {
    std::string version_a;
    std::string version_b;

    std::cout << "Ingrese la primera version: ";
    std::getline(std::cin, version_a);

    std::cout << "Ingrese la segunda version: ";
    std::getline(std::cin, version_b);

    try {
        int resultado =
            comparar_versiones(version_a, version_b);

        if (resultado == -1) {
            std::cout << "La primera version es menor."
                      << std::endl;
        } else if (resultado == 1) {
            std::cout << "La primera version es mayor."
                      << std::endl;
        } else {
            std::cout << "Ambas versiones son iguales."
                      << std::endl;
        }

    } catch (const std::invalid_argument& error) {
        std::cout << "Error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
