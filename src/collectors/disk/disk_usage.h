#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../icollector.hpp"

/// @brief Estructura con información de uso de disco.
struct DiskInfo {
  uint64_t total{};  ///< Espacio total en bytes
  uint64_t used{};   ///< Espacio usado en bytes
  uint64_t free{};   ///< Espacio libre en bytes
};

/// @brief Obtiene información del uso de disco.
/// @return Estructura DiskInfo con espacio total, usado y libre.
/// @note La lectura real se implementa en disk_usage.cpp (getDiskUsage, que
///       consulta statvfs). Un fallo de lectura no devuelve valores inventados:
///       se reporta con chuspapulse::collectors::ErrorRecoleccion, que incluye
///       la ruta consultada y el código errno.
DiskInfo GetDiskUsage();

namespace chuspapulse::collectors::disk {

/// @brief Recolector de métricas de uso de disco.
class DiskCollector : public chuspapulse::collectors::ICollector {
public:
  /// @brief Nombre identificador del recolector.
  /// @return La cadena "disk".
  std::string nombre() const override;

  /// @brief Recolecta las métricas disk.total, disk.used y disk.free (en bytes)
  /// del punto de montaje "/".
  /// @return Vector con las tres métricas.
  /// @throws chuspapulse::collectors::ErrorRecoleccion si falla la lectura del
  ///         disco (statvfs); el mensaje incluye la ruta y el errno. El llamador
  ///         debe capturarla para que el fallo no aborte el proceso.
  std::vector<chuspapulse::core::Metrica> recolectar() override;
};

}  // namespace chuspapulse::collectors::disk