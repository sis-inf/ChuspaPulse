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
DiskInfo GetDiskUsage();

namespace pulso::collectors::disk {

class DiskCollector : public pulso::collectors::ICollector {
public:
  std::string nombre() const override;
  std::vector<pulso::core::Metrica> recolectar() override;
};

}  // namespace pulso::collectors::disk
