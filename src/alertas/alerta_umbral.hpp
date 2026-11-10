#pragma once

#include <string>
#include <stdexcept>
#include "../core/MetricSnapshot.hpp"

namespace chuspapulse::alertas {

/**
 * @brief Alerta que se dispara cuando una métrica supera un umbral configurable.
 *
 * Evalúa una métrica del snapshot contra un umbral numérico usando un operador
 * de comparación ('>' o '<'). Si la condición se cumple, la alerta está activa.
 */
class AlertaUmbral {
public:
    /**
     * @brief Construye una alerta de umbral.
     * @param metrica  Nombre de la métrica a evaluar: cpu, cpu.usage_pct, ram,
     *                 disk, rx_bytes o tx_bytes.
     * @param umbral   Valor de referencia contra el cual comparar.
     * @param operador Operador de comparación: '>' o '<'.
     * @throws std::invalid_argument si la métrica no existe o si el operador
     *         no es '>' ni '<'. El mensaje indica el dato inválido.
     */
    AlertaUmbral(const std::string& metrica, float umbral, char operador);

    /**
     * @brief Evalúa si la condición de alerta se cumple en el snapshot dado.
     * @param snap Snapshot con las métricas actuales del sistema.
     * @return true si la métrica supera el umbral según el operador configurado.
     */
    bool evaluar(const chuspapulse::MetricSnapshot& snap) const;

    /**
     * @brief Devuelve el valor actual de la métrica de esta alerta.
     * @param snap Snapshot con las métricas actuales del sistema.
     * @return Valor del campo del snapshot que corresponde a la métrica.
     */
    double valorActual(const chuspapulse::MetricSnapshot& snap) const;

    /**
     * @brief Devuelve un mensaje descriptivo sobre la alerta.
     * @return Cadena con el nombre de la métrica, el operador y el umbral.
     */
    std::string mensaje() const;

    /**
     * @brief Mensaje que se emite cuando la alerta vuelve a la normalidad.
     * @return Cadena "Alerta resuelta: <métrica> <operador> <umbral>".
     */
    std::string mensajeRecuperacion() const;

    /// Estado de la última evaluación registrada: true si está activa.
    /// Una alerta recién creada empieza en estado normal (false).
    bool activa() const { return activa_; }
    /// Registra el nuevo estado de la alerta (lo usa GestorAlertas).
    void setActiva(bool activa) { activa_ = activa; }

    /// Nombre de la métrica que evalúa esta alerta.
    const std::string& metrica() const { return metrica_; }
    /// Valor umbral de referencia.
    float umbral() const { return umbral_; }
    /// Operador de comparación ('>' o '<').
    char operador() const { return operador_; }

private:
    std::string metrica_;  ///< Nombre de la métrica a evaluar
    float       umbral_;   ///< Valor umbral de referencia
    char        operador_; ///< Operador de comparación: '>' o '<'
    bool        activa_{false}; ///< Estado de la última evaluación
};

} // namespace chuspapulse::alertas
