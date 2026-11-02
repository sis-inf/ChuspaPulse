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

namespace chuspapulse::collectors::disk {

class DiskCollector : public chuspapulse::collectors::ICollector {
public:
  std::string nombre() const override;
  std::vector<chuspapulse::core::Metrica> recolectar() override;
};

}  // namespace chuspapulse::collectors::disk
