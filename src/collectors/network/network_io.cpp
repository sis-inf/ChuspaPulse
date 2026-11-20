#include <fstream>
#include <sstream>
#include <string>
#include <charconv>
#include <limits>
#include <system_error>

#include "utils/logging/logger.hpp"

struct NetworkIO {
    long rx;
    long tx;
};

namespace {

NetworkIO registrarError(const std::string& mensaje) {
    chuspapulse::utils::logging::Logger::instancia().warn(
        "network_io: " + mensaje
    );

    return {-1, -1};
}

bool convertirValor(const std::string& texto, long& valor) {
    const auto resultado = std::from_chars(
        texto.data(),
        texto.data() + texto.size(),
        valor
    );

    return resultado.ec == std::errc{} &&
           resultado.ptr == texto.data() + texto.size() &&
           valor >= 0;
}

NetworkIO leerNetworkIO(const std::string& ruta) {
    std::ifstream file(ruta);

    if (!file.is_open()) {
        return registrarError(
            "Error al acceder a " + ruta + ": archivo no accesible"
        );
    }

    long totalRx = 0;
    long totalTx = 0;
    bool hayInterfaz = false;
    std::string line;

    // Comprobar las dos lineas de encabezado.
    if (!std::getline(file, line) ||
        !std::getline(file, line)) {
        return registrarError(
            "Encabezado incompleto o error de lectura en " + ruta
        );
    }

    while (std::getline(file, line)) {
        if (line.find_first_not_of(" \t\r") == std::string::npos) {
            continue;
        }

        const auto separador = line.find(':');

        if (separador == std::string::npos) {
            return registrarError(
                "Separador de interfaz ausente en " + ruta
            );
        }

        // Nombre de la interfaz sin espacios alrededor.
        std::string iface = line.substr(0, separador);

        const auto inicio = iface.find_first_not_of(" \t");

        if (inicio == std::string::npos) {
            return registrarError(
                "Nombre de interfaz vacio en " + ruta
            );
        }

        iface = iface.substr(
            inicio,
            iface.find_last_not_of(" \t") - inicio + 1
        );

        // Excluir la interfaz de loopback.
        if (iface == "lo") {
            continue;
        }

        // Campo 1: bytes recibidos; campo 9: bytes transmitidos.
        std::istringstream ss(line.substr(separador + 1));

        long rx = 0;
        long tx = 0;

        const std::string contexto = ruta + ": " + iface;

        for (int i = 0; i < 9; ++i) {
            std::string campo;
            long valor = 0;

            if (!(ss >> campo)) {
                return registrarError(
                    "Lectura incompleta en " + contexto
                );
            }

            if (!convertirValor(campo, valor)) {
                return registrarError(
                    "Valor invalido en " + contexto + ": " + campo
                );
            }

            if (i == 0) {
                rx = valor;
            } else if (i == 8) {
                tx = valor;
            }
        }

        if (rx > std::numeric_limits<long>::max() - totalRx) {
            return registrarError(
                "Desbordamiento de bytes RX en " + contexto
            );
        }

        if (tx > std::numeric_limits<long>::max() - totalTx) {
            return registrarError(
                "Desbordamiento de bytes TX en " + contexto
            );
        }

        totalRx += rx;
        totalTx += tx;
        hayInterfaz = true;
    }

    if (file.bad() || !file.eof()) {
        return registrarError(
            "Error durante la lectura de " + ruta
        );
    }

    if (!hayInterfaz) {
        return registrarError(
            "No se encontraron interfaces de red validas en " + ruta
        );
    }

    return {totalRx, totalTx};
}

} // namespace

NetworkIO getNetworkIO() {
    return leerNetworkIO("/proc/net/dev");
}
