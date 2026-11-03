#include "formato_bytes.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <sstream>
#include <iomanip>

namespace chuspapulse::utils {

namespace {

constexpr double kFactor = 1024.0;

constexpr std::array<const char*, 5> kUnidades = {
    "B", "KB", "MB", "GB", "TB"
};

} // namespace

std::string formatearBytesHumanos(double bytes) {
    bool negativo = bytes < 0.0;
    double magnitud = std::fabs(bytes);

    std::size_t indice_unidad = 0;
    const std::size_t max_indice = kUnidades.size() - 1;

    while (magnitud >= kFactor && indice_unidad < max_indice) {
        magnitud /= kFactor;
        ++indice_unidad;
    }

    std::ostringstream oss;
    if (negativo) {
        oss << "-";
    }
    oss << std::fixed << std::setprecision(1)
        << magnitud << " " << kUnidades[indice_unidad];

    return oss.str();
}

} // namespace chuspapulse::utils