
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

bool existe_archivo(const std::string& ruta) {
    if (ruta.empty()) {
        return false;
    }

    std::error_code error;

    bool existe = fs::is_regular_file(ruta, error);

    if (error) {
        return false;
    }

    return existe;
}

int main() {
    std::string ruta;

    std::cout << "Ingrese la ruta del archivo: ";
    std::getline(std::cin, ruta);

    if (existe_archivo(ruta)) {
        std::cout << "El archivo existe." << std::endl;
    } else {
        std::cout << "El archivo no existe o no se puede acceder."
                  << std::endl;
    }

    return 0;
}
