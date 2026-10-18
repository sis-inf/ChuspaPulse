#pragma once

#include "../../collectors/icollector.hpp"
#include <vector>
#include <string>

namespace pulso::collectors {

class CollectorCPU : public ICollector {
public:
    std::string nombre() const override;
    std::vector<Metrica> recolectar() override;
};

} // namespace pulso::collectors