#pragma once

#include "../../core/interfaces/icollector.hpp"
#include <vector>
#include <string>

namespace chuspapulse::collectors {

class CollectorCPU : public ICollector {
public:
    std::string nombre() const override;
    std::vector<Metrica> recolectar() override;
};

} // namespace chuspapulse::collectors
