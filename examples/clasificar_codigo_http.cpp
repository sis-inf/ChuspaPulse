
#include <iostream>
#include <string>

std::string clasificar_codigo_http(int codigo) {
    if (codigo >= 100 && codigo <= 199) {
        return "Informativo";
    }

    if (codigo >= 200 && codigo <= 299) {
        return "Exitoso";
    }

    if (codigo >= 300 && codigo <= 399) {
        return "Redireccion";
    }

    if (codigo >= 400 && codigo <= 499) {
        return "Error del cliente";
    }

    if (codigo >= 500 && codigo <= 599) {
        return "Error del servidor";
    }

    return "Codigo HTTP invalido";
}

int main() {
    int codigo;

    std::cout << "Clasificador de codigos HTTP" << std::endl;
    std::cout << "Ingrese un codigo HTTP: ";
    std::cin >> codigo;

    std::cout << "Categoria: "
              << clasificar_codigo_http(codigo)
              << std::endl;

    return 0;
}
