
#include <iostream>
#include <string>

/**
 * Comprueba si una cadena representa un numero decimal valido.
 *
 * Acepta:
 * - Numeros enteros: 123, -45, +78.
 * - Numeros decimales: 12.5, -3.14, +0.75.
 * - Decimales sin parte entera: .5.
 *
 * @param texto Cadena que se desea validar.
 * @return true si representa un numero valido, false en caso contrario.
 */
bool validar_numero(const std::string& texto) {
    if (texto.empty()) {
        return false;
    }

    std::size_t i = 0;

    // Permitir signo positivo o negativo al inicio.
    if (texto[i] == '+' || texto[i] == '-') {
        ++i;
    }

    // No aceptar una cadena que solo tenga un signo.
    if (i == texto.size()) {
        return false;
    }

    bool tiene_digitos = false;
    bool tiene_punto = false;

    for (; i < texto.size(); ++i) {
        char caracter = texto[i];

        if (caracter >= '0' && caracter <= '9') {
            tiene_digitos = true;
        } else if (caracter == '.' && !tiene_punto) {
            tiene_punto = true;
        } else {
            return false;
        }
    }

    return tiene_digitos;
}

int main() {
    std::string texto;

    std::cout << "Validador de cadenas numericas" << std::endl;
    std::cout << "Ingrese un numero: ";
    std::cin >> texto;

    if (validar_numero(texto)) {
        std::cout << "La cadena representa un numero valido."
                  << std::endl;
    } else {
        std::cout << "La cadena no representa un numero valido."
                  << std::endl;
    }

    return 0;
}
