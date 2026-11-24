
#include <iostream>
#include <string>

/**
 * Normaliza un nombre de archivo.
 *
 * Conserva letras, numeros, puntos, guiones y guiones bajos.
 * Reemplaza los caracteres no permitidos por guiones bajos.
 *
 * @param nombre Nombre de archivo original.
 * @return Nombre normalizado o cadena vacia si no es valido.
 */
std::string sanitizar_nombre_archivo(const std::string& nombre) {
    if (nombre.empty()) {
        return "";
    }

    std::string resultado;

    for (char caracter : nombre) {
        bool es_letra =
            (caracter >= 'a' && caracter <= 'z') ||
            (caracter >= 'A' && caracter <= 'Z');

        bool es_numero = caracter >= '0' && caracter <= '9';

        bool permitido = es_letra || es_numero ||
                         caracter == '.' ||
                         caracter == '-' ||
                         caracter == '_';

        if (permitido) {
            resultado += caracter;
        } else {
            resultado += '_';
        }
    }

    // Evitar nombres especiales.
    if (resultado == "." || resultado == "..") {
        return "";
    }

    return resultado;
}

int main() {
    std::string nombre;

    std::cout << "Sanitizador de nombres de archivos" << std::endl;
    std::cout << "Ingrese un nombre de archivo: ";
    std::getline(std::cin, nombre);

    std::string nombre_normalizado =
        sanitizar_nombre_archivo(nombre);

    if (nombre_normalizado.empty()) {
        std::cout << "Nombre de archivo no valido." << std::endl;
    } else {
        std::cout << "Nombre normalizado: "
                  << nombre_normalizado << std::endl;
    }

    return 0;
}
