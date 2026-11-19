#pragma once

#include "../../collectors/icollector.hpp"
#include <vector>
#include <string>

namespace chuspapulse::collectors {

class CollectorCPU : public ICollector {
public:
    std::string nombre() const override;
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

} // namespace chuspapulse::collectors
