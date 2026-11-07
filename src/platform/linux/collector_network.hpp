#pragma once
#include <string>
#include <vector>
#include "../../collectors/icollector.hpp"
#include "../../collectors/error_recoleccion.hpp"
#include "../../core/types.hpp"

namespace chuspapulse::platform::linux_platform {

/**
 * @brief Implementación Linux del collector de métricas de red.
 *
 * Este collector cumple el contrato definido para CollectorNetwork
 * leyendo estadísticas desde /proc/net/dev.
 *
 * Para cada interfaz distinta de "lo" (loopback), acumula:
 * - bytes recibidos (rx_bytes)
 * - bytes transmitidos (tx_bytes)
 *
 * Las métricas devueltas representan contadores acumulados desde
 * el arranque del sistema.
 *
 * Métricas:
 * - network.rx_bytes
 * - network.tx_bytes
 *
 * Unidad:
 * - bytes
 *
 * @throws ErrorRecoleccion si /proc/net/dev no puede leerse
 *         o tiene un formato inválido.
 */
class CollectorNetworkLinux : public chuspapulse::collectors::ICollector {
public:
    /// @brief Retorna el nombre identificador de este collector.
    /// @return "network"
    std::string nombre() const override;

    /// @brief Recolecta las métricas de red del sistema.
    /// @return Vector con network.rx_bytes y network.tx_bytes en bytes.
    /// @throws chuspapulse::collectors::ErrorRecoleccion si /proc/net/dev no puede abrirse
    ///         o si el formato del archivo es inesperado.
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

} // namespace chuspapulse::platform::linux_platform