#include "formatter_prometheus.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace chuspapulse::formatters {

std::string FormatterPrometheus::formato() const {
    return "prometheus";
}

std::string FormatterPrometheus::contentType() const {
    return "text/plain; version=0.0.4";
}

std::string FormatterPrometheus::formatear(
    const chuspapulse::core::Snapshot& snapshot) const {

    if (snapshot.metricas.empty()) {
        return "";
    }

    std::ostringstream output;
    output << std::setprecision(15);

    for (const auto& metrica : snapshot.metricas) {
        std::string nombre = "chuspapulse_" + metrica.nombre;
        std::replace(nombre.begin(), nombre.end(), '.', '_');

        output << "# HELP " << nombre << " Metrica de chuspapulse\n";
        output << "# TYPE " << nombre << " gauge\n";
        output << nombre << " " << metrica.valor << "\n\n";
    }

    return output.str();
}

std::string FormatterPrometheus::formatearHistorial(
    const std::vector<chuspapulse::core::Snapshot>& snapshots) const {

    if (snapshots.empty()) {
        return "";
    }

    return formatear(snapshots.back());
}

} // namespace chuspapulse::formatters
