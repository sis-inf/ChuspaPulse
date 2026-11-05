#include "proc_collector.h"
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace chuspapulse::collectors {

ProcInfo getProcInfo() {
    ProcInfo info{};

    // =========================
    // Leer /proc/stat
    // =========================
    std::ifstream statFile("/proc/stat");

    if (!statFile.is_open()) {
        throw std::runtime_error(
            "No se pudo abrir /proc/stat para leer información de procesos."
        );
    }

    std::string line;

    while (std::getline(statFile, line)) {
        std::istringstream iss(line);
        std::string key;
        uint32_t value;

        iss >> key >> value;

        if (key == "procs_running") {
            info.running = value;
        } else if (key == "procs_blocked") {
            info.blocked = value;
        }
    }

    statFile.close();

    // =========================
    // Leer /proc/loadavg
    // =========================
    std::ifstream loadavgFile("/proc/loadavg");

    if (!loadavgFile.is_open()) {
        throw std::runtime_error(
            "No se pudo abrir /proc/loadavg para leer el total de procesos."
        );
    }

    std::string load1;
    std::string load5;
    std::string load15;
    std::string procesos;

    loadavgFile >> load1 >> load5 >> load15 >> procesos;

    loadavgFile.close();

    const std::size_t separator = procesos.find('/');

    if (separator == std::string::npos) {
        throw std::runtime_error(
            "Formato inválido en /proc/loadavg: no se encontró 'activos/total'."
        );
    }

    const std::string totalStr = procesos.substr(separator + 1);

    try {
        info.total = static_cast<uint32_t>(std::stoul(totalStr));
    } catch (const std::exception&) {
        throw std::runtime_error(
            "No se pudo convertir el total de procesos desde /proc/loadavg."
        );
    }

    if (info.running > info.total) {
        info.running = info.total;
    }

    return info;
}

} // namespace chuspapulse::collectors
