#include "collector_network.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

namespace chuspapulse::platform::linux_platform {

std::string CollectorNetworkLinux::nombre() const {
    return "network";
}

/**
 * Lee /proc/net/dev y suma los contadores de todas las interfaces
 * excepto la interfaz loopback ("lo").
 *
 * Retorna métricas acumuladas de recepción y transmisión expresadas
 * en bytes.
 */
std::vector<chuspapulse::core::Metrica> CollectorNetworkLinux::recolectar() {
    // /proc/net/dev tiene dos líneas de cabecera antes de los datos:
    //
    //   Inter-|   Receive                                                |  Transmit
    //    face |bytes    packets errs drop fifo frame compressed multicast|bytes    ...
    //       lo:   12345  ...
    //     eth0: 4096000  ...
    //
    // Cada línea de datos tiene el formato:
    //   <iface>: rx_bytes rx_packets rx_errs rx_drop rx_fifo rx_frame
    //            rx_compressed rx_multicast tx_bytes tx_packets ...
    // Los índices de columna (0-based, tras el ":") son:
    //   0  → rx_bytes
    //   8  → tx_bytes

    std::ifstream file("/proc/net/dev");
    if (!file.is_open()) {
        throw chuspapulse::collectors::ErrorRecoleccion(
            "No se pudo abrir /proc/net/dev"
        );
    }

    // Saltar las dos líneas de cabecera
    std::string linea;
    for (int i = 0; i < 2; ++i) {
        if (!std::getline(file, linea)) {
            throw chuspapulse::collectors::ErrorRecoleccion(
                "Formato inesperado en /proc/net/dev: cabecera incompleta"
            );
        }
    }

    long long total_rx_bytes = 0;
    long long total_tx_bytes = 0;
    bool hay_interfaces = false;

    while (std::getline(file, linea)) {
        // Buscar el separador ":" que delimita el nombre de la interfaz
        const std::string::size_type pos_colon = linea.find(':');
        if (pos_colon == std::string::npos) {
            continue; // línea malformada, ignorar
        }

        // Extraer y limpiar el nombre de la interfaz
        std::string iface = linea.substr(0, pos_colon);
        // Eliminar espacios en blanco al inicio y al final del nombre
        const auto inicio = iface.find_first_not_of(" \t");
        const auto fin    = iface.find_last_not_of(" \t");
        if (inicio == std::string::npos) {
            continue; // nombre vacío, ignorar
        }
        iface = iface.substr(inicio, fin - inicio + 1);

        // Excluir la interfaz loopback: no representa tráfico real de red
        if (iface == "lo") {
            continue;
        }

        // Parsear los valores numéricos tras el ":"
        // Orden: rx_bytes rx_packets rx_errs rx_drop rx_fifo rx_frame
        //        rx_compressed rx_multicast tx_bytes tx_packets ...
        std::istringstream iss(linea.substr(pos_colon + 1));
        long long rx_bytes = 0;
        long long campo    = 0;

        // Columna 0: rx_bytes
        if (!(iss >> rx_bytes)) {
            throw chuspapulse::collectors::ErrorRecoleccion(
                "Formato inesperado en /proc/net/dev: no se pudo leer rx_bytes"
                " para la interfaz " + iface
            );
        }

        // Columnas 1-7: rx_packets, rx_errs, rx_drop, rx_fifo,
        //               rx_frame, rx_compressed, rx_multicast
        for (int i = 0; i < 7; ++i) {
            if (!(iss >> campo)) {
                throw chuspapulse::collectors::ErrorRecoleccion(
                    "Formato inesperado en /proc/net/dev: campos RX incompletos"
                    " para la interfaz " + iface
                );
            }
        }

        // Columna 8: tx_bytes
        long long tx_bytes = 0;
        if (!(iss >> tx_bytes)) {
            throw chuspapulse::collectors::ErrorRecoleccion(
                "Formato inesperado en /proc/net/dev: no se pudo leer tx_bytes"
                " para la interfaz " + iface
            );
        }

        total_rx_bytes += rx_bytes;
        total_tx_bytes += tx_bytes;
        hay_interfaces  = true;
    }

    if (!hay_interfaces) {
        throw chuspapulse::collectors::ErrorRecoleccion(
            "No se encontraron interfaces de red activas en /proc/net/dev"
        );
    }

    // Timestamp actual en segundos Unix
    const std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    return {
        {"network.rx_bytes", static_cast<double>(total_rx_bytes), "bytes", ahora},
        {"network.tx_bytes", static_cast<double>(total_tx_bytes), "bytes", ahora},
    };
}

} // namespace chuspapulse::platform::linux_platform