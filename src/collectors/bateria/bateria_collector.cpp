#include "bateria_collector.hpp"

#include <chrono>
#include <fstream>
#include <string>

namespace chuspapulse::collectors::bateria {

BateriaInfo getBateriaInfo(const std::string& base_path) {
    const std::string capacity_path = base_path + "/capacity";
    const std::string status_path   = base_path + "/status";

    std::ifstream capacity_file(capacity_path);
    std::ifstream status_file(status_path);

    // Si no existe BAT0 (caso mas comun: servidores sin bateria), no
    // lanzamos excepcion, simplemente reportamos que no esta disponible.
    if (!capacity_file.is_open() || !status_file.is_open()) {
        return BateriaInfo{
            /* porcentaje */ 0,
            /* estado     */ "",
            /* disponible */ false
        };
    }

    int porcentaje = 0;
    if (!(capacity_file >> porcentaje)) {
        return BateriaInfo{
            /* porcentaje */ 0,
            /* estado     */ "",
            /* disponible */ false
        };
    }

    std::string estado;
    if (!std::getline(status_file, estado) || estado.empty()) {
        return BateriaInfo{
            /* porcentaje */ 0,
            /* estado     */ "",
            /* disponible */ false
        };
    }

    return BateriaInfo{
        /* porcentaje */ porcentaje,
        /* estado     */ estado,
        /* disponible */ true
    };
}

std::string CollectorBateria::nombre() const {
    return "bateria";
}

std::vector<chuspapulse::core::Metrica> CollectorBateria::recolectar() {
    BateriaInfo info = getBateriaInfo();

    // Sin bateria presente: vector vacio, sin metricas vacias ni enganosas.
    if (!info.disponible) {
        return {};
    }

    auto timestamp = static_cast<std::int64_t>(
        std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
        )
    );

    std::vector<chuspapulse::core::Metrica> metricas;

    metricas.push_back({
        "bateria.porcentaje",
        static_cast<double>(info.porcentaje),
        "porcentaje",
        timestamp
    });

    // "Charging" -> 1.0, cualquier otro estado (Discharging, Full,
    // Not charging, Unknown) -> 0.0.
    double cargando = (info.estado == "Charging") ? 1.0 : 0.0;

    metricas.push_back({
        "bateria.cargando",
        cargando,
        "booleano",
        timestamp
    });

    return metricas;
}

} // namespace chuspapulse::collectors::bateria