
#include <iostream>
#include <string>
#include <sstream>

std::size_t contar_palabras(const std::string& texto) {
    std::istringstream flujo(texto);
    std::string palabra;
    std::size_t contador = 0;

    while (flujo >> palabra) {
        contador++;
    }

    return contador;
}

int main() {
    std::string texto;

    std::cout << "Ingrese un texto: ";
    std::getline(std::cin, texto);

    std::size_t resultado = contar_palabras(texto);

    std::cout << "Cantidad de palabras: "
              << resultado << std::endl;

    return 0;
}
