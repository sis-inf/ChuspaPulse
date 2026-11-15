#include "formatter_json.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace chuspapulse::formatters {

std::string FormatterJSON::formato() const {
    return "json";
}

std::string FormatterJSON::contentType() const {
    return "application/json";
}

std::string FormatterJSON::formatear(
    const chuspapulse::core::Snapshot& snapshot
) const {

    json resultado;

    resultado["timestamp"] = snapshot.timestamp;
    resultado["metricas"] = json::array();

    for (const auto& metrica : snapshot.metrics) {
        resultado["metricas"].push_back({
            {"nombre", metrica.name},
            {"valor", metrica.value},
            {"unidad", metrica.unit},
            {"timestamp", metrica.timestamp}
        });
    }

    return resultado.dump();
}

std::string FormatterJSON::formatearHistorial(
    const std::vector<chuspapulse::core::Snapshot>& snapshots
) const {

    json resultado;

    resultado["total"] = snapshots.size();
    resultado["snapshots"] = json::array();

    for (const auto& snapshot : snapshots) {

        json item;

        item["timestamp"] = snapshot.timestamp;
        item["metricas"] = json::array();

        for (const auto& metrica : snapshot.metrics) {
            item["metricas"].push_back({
                {"nombre", metrica.name},
                {"valor", metrica.value},
                {"unidad", metrica.unit},
                {"timestamp", metrica.timestamp}
            });
        }

        resultado["snapshots"].push_back(item);
    }

    return resultado.dump();
}

}