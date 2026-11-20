#include "temp_collector.h"
#include "utils/logging/logger.hpp"

#include <fstream>
#include <string>

namespace collectors {

namespace {

constexpr const char* kThermalZonePath =
    "/sys/class/thermal/thermal_zone0/temp";

constexpr const char* kZoneName = "thermal_zone0";

constexpr float kMilliCelsiusDivisor = 1000.0f;

// Rango fisico plausible para una lectura de temperatura (grados C).
constexpr float kMinCelsiusPlausible = 0.0f;
constexpr float kMaxCelsiusPlausible = 150.0f;

TempInfo temperaturaNoDisponible(const std::string& motivo)
{
    chuspapulse::utils::logging::Logger::instancia().warn(
        std::string("temp_collector: ") + motivo +
        " [" + kThermalZonePath + "]"
    );

    return TempInfo{
        /* cpu_celsius */ 0.0f,
        /* zona        */ kZoneName,
        /* disponible  */ false
    };
}

} // namespace

TempInfo getTempInfo()
{
    std::ifstream file(kThermalZonePath);

    if (!file.is_open()) {
        return temperaturaNoDisponible(
            "No se pudo abrir el archivo de temperatura"
        );
    }

    long raw_millicelsius = 0;
    if (!(file >> raw_millicelsius)) {
        return temperaturaNoDisponible(
            "Error al leer el valor de temperatura"
        );
    }

    const float celsius =
        static_cast<float>(raw_millicelsius) / kMilliCelsiusDivisor;

    // Descartar lecturas fuera del rango fisico plausible.
    if (celsius < kMinCelsiusPlausible || celsius > kMaxCelsiusPlausible) {
        return temperaturaNoDisponible(
            "Temperatura fuera del rango plausible: " +
            std::to_string(celsius) + " C"
        );
    }

    return TempInfo{
        /* cpu_celsius */ celsius,
        /* zona        */ kZoneName,
        /* disponible  */ true
    };
}

} // namespace collectors
