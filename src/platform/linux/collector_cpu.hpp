#pragma once

#include <string>
#include <vector>

#include "../../collectors/icollector.hpp"
#include "../../core/types.hpp"

namespace chuspapulse::collectors {

class CollectorCPU : public ICollector {
public:
    std::string nombre() const override;
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

} // namespace chuspapulse::collectors
