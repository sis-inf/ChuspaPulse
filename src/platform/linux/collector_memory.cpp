#include "collector_memory.hpp"

#include <fstream>
#include <string>
#include <ctime>

namespace chuspapulse::platform::linux_platform {

std::string CollectorMemoryLinux::nombre() const {
    return "memory";
}

std::vector<chuspapulse::core::Metrica> CollectorMemoryLinux::recolectar() {
    // /proc/meminfo tiene el formato:
    //   FieldName:   <valor> kB
    // Se leen línea por línea buscando MemTotal y MemAvailable.
    // Ambos campos están presentes en cualquier kernel Linux >= 3.14.

    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) {
        throw chuspapulse::collectors::ErrorRecoleccion(
            "No se pudo abrir /proc/meminfo"
        );
    }

    long long memTotalKB     = -1;
    long long memAvailableKB = -1;

    std::string key;
    long long   value;
    std::string unit;

    // Parseo directo: cada línea tiene la forma "Clave:   valor kB"
    while (file >> key >> value >> unit) {
        if (key == "MemTotal:") {
            memTotalKB = value;
        } else if (key == "MemAvailable:") {
            memAvailableKB = value;
        }

        // Salida temprana: ya tenemos los dos campos que necesitamos
        if (memTotalKB >= 0 && memAvailableKB >= 0) {
            break;
        }
    }

    if (memTotalKB < 0) {
        throw chuspapulse::collectors::ErrorRecoleccion(
            "No se encontró el campo MemTotal en /proc/meminfo"
        );
    }
    if (memAvailableKB < 0) {
        throw chuspapulse::collectors::ErrorRecoleccion(
            "No se encontró el campo MemAvailable en /proc/meminfo"
        );
    }

    // Convertir de kB a bytes
    const double totalBytes     = static_cast<double>(memTotalKB)     * 1024.0;
    const double availableBytes = static_cast<double>(memAvailableKB) * 1024.0;

    // used = MemTotal - MemAvailable
    // Invariante garantizada por el kernel: MemAvailable <= MemTotal,
    // por lo tanto used_bytes >= 0 y used_bytes <= total_bytes siempre.
    const double usedBytes = totalBytes - availableBytes;

    const std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    return {
        {"memory.total_bytes",     totalBytes,     "bytes", ahora},
        {"memory.used_bytes",      usedBytes,      "bytes", ahora},
        {"memory.available_bytes", availableBytes, "bytes", ahora},
    };
}

} // namespace chuspapulse::platform::linux_platform