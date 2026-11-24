
#include <string>
#include <algorithm>
#include <cctype>

/**
 * Obtiene la extension de un nombre de archivo.
 *
 * Extrae el texto despues del ultimo punto,
 * convierte la extension a minusculas y contempla
 * nombres sin extension.
 *
 * @param nombre_archivo Nombre del archivo.
 * @return Extension en minusculas o cadena vacia.
 */
std::string obtener_extension(const std::string& nombre_archivo) {
    if (nombre_archivo.empty()) {
        return "";
    }

    // Buscar el ultimo separador de directorio.
    const std::size_t separador =
        nombre_archivo.find_last_of("/\\");

    // Buscar el ultimo punto.
    const std::size_t punto =
        nombre_archivo.find_last_of('.');

    // No existe punto o pertenece al directorio.
    if (punto == std::string::npos ||
        (separador != std::string::npos && punto < separador)) {
        return "";
    }

    // Los archivos ocultos como .gitignore no tienen extension.
    const std::size_t inicio_nombre =
        (separador == std::string::npos) ? 0 : separador + 1;

    if (punto == inicio_nombre) {
        return "";
    }

    // No considerar extension si el nombre termina en punto.
    if (punto == nombre_archivo.length() - 1) {
        return "";
    }

    // Extraer la extension sin incluir el punto.
    std::string extension = nombre_archivo.substr(punto + 1);

    // Convertir la extension a minusculas.
    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char caracter) {
            return static_cast<char>(std::tolower(caracter));
        }
    );

    return extension;
}
