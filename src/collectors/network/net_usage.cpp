#include "net_usage.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <ctime>

/// @brief Estructura que representa el uso de red del sistema.
/// Acumula los bytes recibidos y enviados de todas las interfaces
/// activas, excluyendo la interfaz de loopback (lo).
struct NetUsage {
    uint64_t rx_bytes_total{}; ///< Total de bytes recibidos por todas las interfaces
    uint64_t tx_bytes_total{}; ///< Total de bytes enviados por todas las interfaces
};

/// @brief Obtiene los bytes recibidos y enviados por la red desde /proc/net/dev.
/// Salta las dos primeras líneas de encabezado, parsea cada interfaz extrayendo
/// el nombre (antes de ':'), columna 1 = rx_bytes y columna 9 = tx_bytes.
/// Excluye la interfaz lo y suma el resto de interfaces.
/// @param ruta Ruta al archivo de estadísticas de red (por defecto "/proc/net/dev").
/// @return Estructura NetUsage con rx_bytes_total y tx_bytes_total.
/// @throws std::runtime_error si el archivo no es accesible.
NetUsage getNetUsage(const std::string& ruta = "/proc/net/dev") {
    std::ifstream archivo(ruta);

    if (!archivo.is_open()) {
        throw std::runtime_error(
            "Error al acceder a " + ruta + ": archivo no accesible"
        );
    }

    NetUsage uso{};
    std::string linea;

    std::getline(archivo, linea);
    std::getline(archivo, linea); 

    while (std::getline(archivo, linea)) {
        std::istringstream flujo(linea);

        std::string nombre_interfaz;
        std::getline(flujo, nombre_interfaz, ':');

        size_t inicio = nombre_interfaz.find_first_not_of(" \t");
        if (inicio != std::string::npos) {
            nombre_interfaz = nombre_interfaz.substr(inicio);
        }

        if (nombre_interfaz == "lo") {
            continue;
        }

        uint64_t rx_bytes = 0;
        uint64_t ignorado = 0;
        uint64_t tx_bytes = 0;

        flujo >> rx_bytes;

        for (int i = 0; i < 7; ++i) {
            flujo >> ignorado;
        }

        flujo >> tx_bytes;

        uso.rx_bytes_total += rx_bytes;
        uso.tx_bytes_total += tx_bytes;
    }

    return uso;
}

namespace pulso::collectors::network {

std::string NetworkCollector::nombre() const {
    return "network";
}

std::vector<pulso::core::Metrica> NetworkCollector::recolectar() {
    const NetUsage uso = getNetUsage("/proc/net/dev");
    const std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    return {
        {"net.rx", static_cast<double>(uso.rx_bytes_total), "bytes", ahora},
        {"net.tx", static_cast<double>(uso.tx_bytes_total), "bytes", ahora},
    };
}

}  // namespace pulso::collectors::network
