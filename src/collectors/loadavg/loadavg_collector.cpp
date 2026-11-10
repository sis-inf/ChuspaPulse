#include "loadavg_collector.h"
#include <fstream>
#include <stdexcept>
#include <cmath>

// Función auxiliar para asegurar precisión de dos decimales
static float redondearDosDecimales(float val) {
    return std::round(val * 100.0f) / 100.0f;
}

LoadAvgInfo getLoadAverage() {
    // Abrir el archivo virtual del sistema operativo Linux
    std::ifstream file("/proc/loadavg");
    
    // Criterio de aceptación: Lanzar std::runtime_error si no existe o no se puede abrir
    if (!file.is_open()) {
        throw std::runtime_error("Error: No se pudo abrir o no existe el archivo /proc/loadavg");
    }
    
    LoadAvgInfo info{0.0f, 0.0f, 0.0f};
    
    // Leer y parsear los primeros 3 valores float separados por espacios en blanco
    if (!(file >> info.load1 >> info.load5 >> info.load15)) {
        throw std::runtime_error("Error: El formato de /proc/loadavg es invalido");
    }
    
    // Normalizar asegurando precisión de dos decimales
    info.load1 = redondearDosDecimales(info.load1);
    info.load5 = redondearDosDecimales(info.load5);
    info.load15 = redondearDosDecimales(info.load15);
    
    return info;
}

namespace chuspapulse::collectors {

std::vector<chuspapulse::core::Metrica> LoadAvgCollector::colectar() const {
    LoadAvgInfo info = getLoadAverage();
    return {
        {"system.loadavg.1m", static_cast<double>(info.load1)},
        {"system.loadavg.5m", static_cast<double>(info.load5)},
        {"system.loadavg.15m", static_cast<double>(info.load15)}
    };
}

} // namespace chuspapulse::collectors
