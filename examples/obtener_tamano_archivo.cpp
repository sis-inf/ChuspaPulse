
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>
#include <optional>

namespace fs = std::filesystem;

std::optional<std::uintmax_t> obtener_tamano_archivo(
    const std::string& ruta) {

    if (ruta.empty()) {
        return std::nullopt;
    }

    std::error_code error;

    if (!fs::is_regular_file(ruta, error) || error) {
        return std::nullopt;
    }

    std::uintmax_t tamano = fs::file_size(ruta, error);

    if (error) {
        return std::nullopt;
    }

    return tamano;
}

int main() {
    std::string ruta;

    std::cout << "Ingrese la ruta del archivo: ";
    std::getline(std::cin, ruta);

    std::optional<std::uintmax_t> resultado =
        obtener_tamano_archivo(ruta);

    if (resultado.has_value()) {
        std::cout << "Tamano del archivo: "
                  << resultado.value() << " bytes"
                  << std::endl;
    } else {
        std::cout << "No se pudo obtener el tamano del archivo."
                  << std::endl;
    }

    return 0;
}
