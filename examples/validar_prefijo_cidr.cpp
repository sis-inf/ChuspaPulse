
#include <iostream>

/**
 * Valida si un prefijo CIDR es correcto para IPv4.
 *
 * @param prefijo Longitud del prefijo CIDR.
 * @return true si el prefijo esta entre 0 y 32.
 */
bool validar_prefijo_cidr(int prefijo) {
    return prefijo >= 0 && prefijo <= 32;
}

int main() {
    int prefijo;

    std::cout << "Validador de prefijos CIDR IPv4" << std::endl;
    std::cout << "Ingrese la longitud del prefijo: ";
    std::cin >> prefijo;

    if (validar_prefijo_cidr(prefijo)) {
        std::cout << "Prefijo CIDR valido: /"
                  << prefijo << std::endl;
    } else {
        std::cout << "Error: el prefijo debe estar entre 0 y 32."
                  << std::endl;
    }

    return 0;
}
