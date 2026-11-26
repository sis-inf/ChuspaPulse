
#include <iostream>
#include <string>

std::string obtener_sistema_operativo() {
#if defined(_WIN32) || defined(_WIN64)
    return "Windows";
#elif defined(__APPLE__) && defined(__MACH__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#else
    return "Sistema operativo desconocido";
#endif
}

int main() {
    std::cout << "Detector de sistema operativo" << std::endl;

    std::cout << "Sistema operativo identificado: "
              << obtener_sistema_operativo()
              << std::endl;

    return 0;
}
