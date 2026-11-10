#include "formatter_prometheus.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace chuspapulse::formatters {

namespace {

/// Muestras de una misma métrica, en el orden en que aparecen.
struct FamiliaMetrica {
    std::string nombre;
    std::vector<std::string> muestras;
};

} // namespace

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

    // Agrupar las muestras por métrica para declarar HELP y TYPE una sola vez.
    std::vector<FamiliaMetrica> familias;
    std::unordered_map<std::string, std::size_t> indices;

    for (const auto& metrica : snapshot.metricas) {
        const std::size_t inicioEtiquetas = metrica.nombre.find('{');

        std::string nombre =
            "chuspapulse_" + metrica.nombre.substr(0, inicioEtiquetas);
        std::replace(nombre.begin(), nombre.end(), '.', '_');

        const std::string etiquetas =
            inicioEtiquetas == std::string::npos
                ? ""
                : metrica.nombre.substr(inicioEtiquetas);

        std::ostringstream muestra;
        muestra << std::setprecision(15)
                << nombre << etiquetas << " " << metrica.valor;

        auto [it, nueva] = indices.try_emplace(nombre, familias.size());
        if (nueva) {
            familias.push_back({nombre, {}});
        }

        familias[it->second].muestras.push_back(muestra.str());
    }

    std::ostringstream output;

    for (std::size_t i = 0; i < familias.size(); ++i) {
        if (i > 0) {
            output << "\n";
        }

        output << "# HELP " << familias[i].nombre << " Metrica de chuspapulse\n";
        output << "# TYPE " << familias[i].nombre << " gauge\n";

        for (const auto& muestra : familias[i].muestras) {
            output << muestra << "\n";
        }
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
