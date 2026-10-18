#include "formatter_json.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace pulso::formatters {

std::string FormatterJSON::formato() const {
    return "json";
}

std::string FormatterJSON::contentType() const {
    return "application/json";
}

std::string FormatterJSON::formatear(
    const pulso::core::Snapshot& snapshot
) const {

    json resultado;

    resultado["timestamp"] = snapshot.timestamp;
    resultado["metricas"] = json::array();

    for (const auto& metrica : snapshot.metricas) {
        resultado["metricas"].push_back({
            {"nombre", metrica.nombre},
            {"valor", metrica.valor},
            {"unidad", metrica.unidad},
            {"timestamp", metrica.timestamp}
        });
    }

    return resultado.dump();
}

std::string FormatterJSON::formatearHistorial(
    const std::vector<pulso::core::Snapshot>& snapshots
) const {

    json resultado;

    resultado["total"] = snapshots.size();
    resultado["snapshots"] = json::array();

    for (const auto& snapshot : snapshots) {

        json item;

        item["timestamp"] = snapshot.timestamp;
        item["metricas"] = json::array();

        for (const auto& metrica : snapshot.metricas) {
            item["metricas"].push_back({
                {"nombre", metrica.nombre},
                {"valor", metrica.valor},
                {"unidad", metrica.unidad},
                {"timestamp", metrica.timestamp}
            });
        }

        resultado["snapshots"].push_back(item);
    }

    return resultado.dump();
}

}