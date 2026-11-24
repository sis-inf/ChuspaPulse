#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "../icollector.hpp"

/// @brief Bytes recibidos y transmitidos por las interfaces de red del sistema.
typedef struct {
    uint64_t rx_bytes;  ///< Bytes recibidos
    uint64_t tx_bytes;  ///< Bytes transmitidos
} NetInfo;

/// @brief Obtiene el uso de red del sistema.
/// @return Estructura NetInfo con los bytes recibidos y transmitidos.
/// @note La lectura real se implementa en net_usage.cpp (getNetUsage), que suma
///       las interfaces de /proc/net/dev excluyendo "lo". Un fallo de lectura no
///       devuelve valores inventados: se registra un mensaje descriptivo con el
///       logger (nivel warn) y se reporta con chuspapulse::collectors::ErrorRecoleccion
///       (archivo no accesible, encabezado incompleto, separador o nombre de
///       interfaz ausente, campos incompletos o inválidos, desbordamiento de
///       bytes RX/TX o error durante la lectura).
NetInfo getNetUsage();

namespace chuspapulse::collectors::network {

/// @brief Recolector de métricas de uso de red.
class NetworkCollector : public chuspapulse::collectors::ICollector {
public:
    /// @brief Nombre identificador del recolector.
    /// @return La cadena "network".
    std::string nombre() const override;

    /// @brief Recolecta las métricas net.rx y net.tx (en bytes) leídas de
    /// /proc/net/dev.
    /// @return Vector con las dos métricas.
    /// @throws chuspapulse::collectors::ErrorRecoleccion si falla la lectura de
    ///         las estadísticas de red; el mensaje ya se registró en el logger.
    ///         El llamador debe capturarla para que el fallo no aborte el proceso.
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

}  // namespace chuspapulse::collectors::network
