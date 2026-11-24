
#include <string>

/**
 * Convierte las letras ASCII mayusculas de una cadena
 * a minusculas, conservando numeros y caracteres especiales.
 *
 * La cadena original no se modifica.
 *
 * @param texto Cadena de texto que se desea convertir.
 * @return Una copia del texto convertido a minusculas.
 */
std::string convertir_minusculas(const std::string& texto) {
    std::string resultado = texto;

    for (char& caracter : resultado) {
        if (caracter >= 'A' && caracter <= 'Z') {
            caracter = static_cast<char>(
                caracter - 'A' + 'a'
            );
        }
    }

    return resultado;
}
