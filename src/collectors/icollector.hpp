#pragma once
#include <string>
#include <vector>
#include "core/types.hpp"

namespace chuspapulse::collectors {

/**
 * @brief Interfaz abstracta común para todos los recolectores de métricas.
 *
 * Cada categoría de métrica (CPU, memoria, disco, red) se implementa como
 * un collector distinto que hereda de esta interfaz. Esto permite al sampler
 * iterar sobre todos los collectors sin conocer sus detalles internos.
 */
class ICollector {
 public:
  virtual ~ICollector() = default;

  /**
   * @brief Devuelve el nombre identificador del collector.
   * @return Nombre del collector como cadena de texto.
   */
  virtual std::string nombre() const = 0;

  /**
   * @brief Ejecuta una medición y devuelve las métricas obtenidas.
   * @return Vector de chuspapulse::core::Metrica con las métricas recolectadas.
   * @throws chuspapulse::core::ErrorRecoleccion si la medición falla.
   */
  virtual std::vector<chuspapulse::core::Metrica> recolectar() = 0;
};

}  // namespace chuspapulse::collectors
