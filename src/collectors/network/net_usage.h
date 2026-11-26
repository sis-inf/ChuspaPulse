#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "../icollector.hpp"

/// @brief Totales de bytes recibidos y transmitidos por las interfaces de red
/// (rx_bytes_total y tx_bytes_total), excluyendo la interfaz de loopback "lo".
/// La estructura se define en net_usage.cpp, junto con getNetUsage.
struct NetUsage;

/// @brief Obtiene el uso de red del sistema leyendo /proc/net/dev.
/// Suma los bytes recibidos y transmitidos de todas las interfaces excepto "lo".
/// @param ruta Ruta al archivo de estadísticas de red. net_usage.cpp define su
///        valor por defecto ("/proc/net/dev").
/// @return NetUsage con los totales de bytes recibidos y transmitidos.
/// @throws chuspapulse::collectors::ErrorRecoleccion si falla la recolección:
///         archivo no accesible, encabezado incompleto, separador o nombre de
///         interfaz ausente, campos incompletos o inválidos, desbordamiento de
///         bytes RX/TX o error durante la lectura.
/// @note Un fallo no devuelve valores inventados: antes de lanzar la excepción
///       se registra un mensaje descriptivo con el logger (nivel warn).
NetUsage getNetUsage(const std::string& ruta);

namespace chuspapulse::collectors::network {

/// @brief Recolector de métricas de uso de red.
class NetworkCollector : public chuspapulse::collectors::ICollector {
public:
    /// @brief Nombre identificador del recolector.
    /// @return La cadena "network".
    std::string nombre() const override;

    /// @brief Recolecta las métricas net.rx y net.tx (en bytes) leídas de
    /// /proc/net/dev mediante getNetUsage.
    /// @return Vector con las dos métricas.
    /// @throws chuspapulse::collectors::ErrorRecoleccion si falla la lectura de
    ///         las estadísticas de red. El recolector no captura la excepción,
    ///         por lo que el llamador debe capturarla para que el fallo no
    ///         termine el proceso.
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

}  // namespace chuspapulse::collectors::network
