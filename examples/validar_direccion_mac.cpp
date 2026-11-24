
#include <regex>
#include <string>

/**
 * Valida si una cadena representa una direccion MAC.
 *
 * Acepta seis grupos de dos caracteres hexadecimales,
 * separados mediante dos puntos o guiones.
 *
 * @param direccion Cadena que contiene la direccion MAC.
 * @return true si la direccion es valida; false en caso contrario.
 */
bool validar_direccion_mac(const std::string& direccion) {
    static const std::regex formato_mac(
        R"(^([0-9A-Fa-f]{2}(:[0-9A-Fa-f]{2}){5}|[0-9A-Fa-f]{2}(-[0-9A-Fa-f]{2}){5})$)"
    );

    return std::regex_match(direccion, formato_mac);
}
