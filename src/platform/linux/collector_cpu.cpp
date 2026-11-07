#include "collector_cpu.hpp"
#include "../../core/error_recoleccion.hpp"

#include <fstream>
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>
#include <cctype>
#include <string>

namespace chuspapulse::collectors {

// Estructura interna para leer /proc/stat
struct CPUStat {
    unsigned long user = 0;
    unsigned long nice = 0;
    unsigned long system = 0;
    unsigned long idle = 0;
    unsigned long iowait = 0;
    unsigned long irq = 0;
    unsigned long softirq = 0;
    unsigned long steal = 0;
};

// Leer primera linea de /proc/stat
CPUStat leerCPU() {
    std::ifstream file("/proc/stat");

    if (!file.is_open()) {
        throw ErrorRecoleccion("No se pudo abrir /proc/stat");
    }

    std::string line;
    std::getline(file, line);

    std::istringstream ss(line);

    std::string cpu;
    CPUStat stat;

    ss >> cpu >> stat.user >> stat.nice >> stat.system
       >> stat.idle >> stat.iowait >> stat.irq
       >> stat.softirq >> stat.steal;

    return stat;
}

// Calcular uso CPU entre dos mediciones
double calcularUso(const CPUStat& a, const CPUStat& b) {
    unsigned long idleA = a.idle + a.iowait;
    unsigned long idleB = b.idle + b.iowait;

    unsigned long totalA = a.user + a.nice + a.system + a.idle +
                           a.iowait + a.irq + a.softirq + a.steal;
    unsigned long totalB = b.user + b.nice + b.system + b.idle +
                           b.iowait + b.irq + b.softirq + b.steal;

    unsigned long totalDelta = totalB - totalA;
    unsigned long idleDelta  = idleB  - idleA;

    if (totalDelta == 0) return 0;

    return 100.0 * (1.0 - (double)idleDelta / totalDelta);
}

// Contar cores CPU
int contarCores() {
    std::ifstream file("/proc/stat");

    if (!file.is_open()) {
        throw ErrorRecoleccion("No se pudo abrir /proc/stat");
    }

    std::string line;
    int cores = 0;

    while (std::getline(file, line)) {
        if (line.rfind("cpu", 0) == 0 && std::isdigit(static_cast<unsigned char>(line[3]))) {
            cores++;
        }
    }

    return cores;
}

// Nombre del collector
std::string CollectorCPU::nombre() const {
    return "cpu";
}

// Función principal
std::vector<chuspapulse::core::Metrica> CollectorCPU::recolectar() {
    CPUStat a = leerCPU();

    std::this_thread::sleep_for(std::chrono::milliseconds(120));

    CPUStat b = leerCPU();

    double usage = calcularUso(a, b);
    int cores    = contarCores();

    if (usage < 0)   usage = 0;
    if (usage > 100) usage = 100;

    return {
        chuspapulse::core::Metrica{"cpu.usage", usage, "porcentaje"},
        chuspapulse::core::Metrica{"cpu.cores", cores, "cantidad"}
    };
}

} // namespace chuspapulse::collectors