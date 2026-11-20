#include "net_usage.h"
#include "../error_recoleccion.hpp"
#include "utils/logging/logger.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <ctime>
#include <charconv>
#include <limits>
#include <system_error>

/// @brief Estructura que representa el uso de red del sistema.
/// Acumula los bytes recibidos y enviados, excluyendo loopback.
struct NetUsage {
    uint64_t rx_bytes_total{};
    uint64_t tx_bytes_total{};
};

namespace {

[[noreturn]] void registrarError(const std::string& mensaje) {
    const std::string detalle = "net_usage: " + mensaje;

    chuspapulse::utils::logging::Logger::instancia().warn(detalle);

    throw chuspapulse::collectors::ErrorRecoleccion(detalle);
}

uint64_t convertirValor(const std::string& texto,
                        const std::string& contexto) {
    uint64_t valor = 0;

    const auto resultado = std::from_chars(
        texto.data(),
        texto.data() + texto.size(),
        valor
    );

    if (resultado.ec != std::errc{} ||
        resultado.ptr != texto.data() + texto.size()) {
        registrarError("Valor invalido en " + contexto + ": " + texto);
    }

    return valor;
}

} // namespace

/// @brief Obtiene los bytes recibidos y enviados desde /proc/net/dev.
/// Excluye la interfaz lo y suma el resto de interfaces.
/// @param ruta Ruta al archivo de estadisticas de red.
/// @return Totales de bytes recibidos y enviados.
/// @throws chuspapulse::collectors::ErrorRecoleccion si falla la recoleccion.
NetUsage getNetUsage(const std::string& ruta = "/proc/net/dev") {
    std::ifstream archivo(ruta);

    if (!archivo.is_open()) {
        registrarError(
            "Error al acceder a " + ruta + ": archivo no accesible"
        );
    }

    NetUsage uso{};
    std::string linea;

    if (!std::getline(archivo, linea) ||
        !std::getline(archivo, linea)) {
        registrarError("Encabezado incompleto en " + ruta);
    }

    while (std::getline(archivo, linea)) {
        if (linea.find_first_not_of(" \t\r") == std::string::npos) {
            continue;
        }

        const auto separador = linea.find(':');

        if (separador == std::string::npos) {
            registrarError("Separador de interfaz ausente en " + ruta);
        }

        std::string nombre_interfaz = linea.substr(0, separador);

        const auto inicio = nombre_interfaz.find_first_not_of(" \t");
        const auto fin = nombre_interfaz.find_last_not_of(" \t");

        if (inicio == std::string::npos) {
            registrarError("Nombre de interfaz vacio en " + ruta);
        }

        nombre_interfaz = nombre_interfaz.substr(
            inicio, fin - inicio + 1
        );

        if (nombre_interfaz == "lo") {
            continue;
        }

        std::istringstream flujo(linea.substr(separador + 1));

        uint64_t rx_bytes = 0;
        uint64_t tx_bytes = 0;

        const std::string contexto = ruta + ": " + nombre_interfaz;

        for (int i = 0; i < 9; ++i) {
            std::string campo;

            if (!(flujo >> campo)) {
                registrarError("Lectura incompleta en " + contexto);
            }

            const uint64_t valor = convertirValor(campo, contexto);

            if (i == 0) {
                rx_bytes = valor;
            } else if (i == 8) {
                tx_bytes = valor;
            }
        }

        if (rx_bytes > std::numeric_limits<uint64_t>::max() -
                           uso.rx_bytes_total) {
            registrarError("Desbordamiento de bytes RX en " + contexto);
        }

        if (tx_bytes > std::numeric_limits<uint64_t>::max() -
                           uso.tx_bytes_total) {
            registrarError("Desbordamiento de bytes TX en " + contexto);
        }

        uso.rx_bytes_total += rx_bytes;
        uso.tx_bytes_total += tx_bytes;
    }

    if (archivo.bad() || !archivo.eof()) {
        registrarError("Error durante la lectura de " + ruta);
    }

    return uso;
}

namespace chuspapulse::collectors::network {

std::string NetworkCollector::nombre() const {
    return "network";
}

std::vector<chuspapulse::core::Metrica> NetworkCollector::recolectar() {
    const NetUsage uso = getNetUsage("/proc/net/dev");
    const std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    return {
        {"net.rx", static_cast<double>(uso.rx_bytes_total), "bytes", ahora},
        {"net.tx", static_cast<double>(uso.tx_bytes_total), "bytes", ahora},
    };
}

}  // namespace chuspapulse::collectors::network
