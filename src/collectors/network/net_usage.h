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

namespace chuspapulse::collectors::network {

class NetworkCollector : public chuspapulse::collectors::ICollector {
public:
    std::string nombre() const override;
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

}  // namespace chuspapulse::collectors::network
