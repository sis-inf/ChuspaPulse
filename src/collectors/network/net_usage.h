#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "../icollector.hpp"

typedef struct {
    uint64_t rx_bytes;
    uint64_t tx_bytes;
} NetInfo;

NetInfo getNetUsage();

namespace pulso::collectors::network {

class NetworkCollector : public pulso::collectors::ICollector {
public:
    std::string nombre() const override;
    std::vector<pulso::core::Metrica> recolectar() override;
};

}  // namespace pulso::collectors::network
