#pragma once

#include <string>
#include <vector>

#include "iformatter.hpp"

namespace chuspapulse::formatters {

class FormatterPrometheus : public IFormatter {
public:
    std::string formato() const override;

    std::string contentType() const override;

    std::string formatear(
        const chuspapulse::core::Snapshot& snapshot) const override;

    std::string formatearHistorial(
        const std::vector<chuspapulse::core::Snapshot>& snapshots) const override;
};

} // namespace chuspapulse::formatters
