#include "ram_usage.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <chrono>

namespace chuspapulse::collectors::memory {

RamInfo getRamUsage(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error(
            "No se pudo abrir el archivo " + path + ": "
            "verifique permisos y disponibilidad del sistema"
        );
    }

    std::string line;
    long long memTotal     = 0;
    long long memAvailable = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        long long value;

        iss >> key >> value;

        if (key == "MemTotal:") {
            memTotal = value;
        }

        if (key == "MemAvailable:") {
            memAvailable = value;
        }
    }

    if (memTotal == 0 || memAvailable == 0) {
        throw std::runtime_error(
            "No se encontraron las claves MemTotal o MemAvailable en " + path
        );
    }

    // Convertir de kB a bytes
    memTotal     *= 1024;
    memAvailable *= 1024;

    // Calcular memoria usada: MemTotal - MemAvailable
    long long memUsed = memTotal - memAvailable;

    return {
        static_cast<uint64_t>(memTotal),
        static_cast<uint64_t>(memUsed),
        static_cast<uint64_t>(memAvailable)
    };
}

std::string CollectorMemory::nombre() const {
    return "memory";
}

std::vector<chuspapulse::core::Metrica> CollectorMemory::recolectar() {

    RamInfo ram = getRamUsage();

    auto timestamp = static_cast<std::int64_t>(
        std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
        )
    );

    return {
        {
            "memory.used",
            static_cast<double>(ram.used),
            "bytes",
            timestamp
        }
    };
}

} // namespace chuspapulse::collectors::memory
