#include "collector_cpu.hpp"
#include "../../collectors/error_recoleccion.hpp"

#include <cctype>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace chuspapulse::collectors {

#if defined(__linux__)

namespace {

// Estructura interna para leer /proc/stat con tipos de ancho fijo estándar.
struct CPUStat {
  std::uint64_t user = 0;
  std::uint64_t nice = 0;
  std::uint64_t system = 0;
  std::uint64_t idle = 0;
  std::uint64_t iowait = 0;
  std::uint64_t irq = 0;
  std::uint64_t softirq = 0;
  std::uint64_t steal = 0;
};

// Leer primera linea de /proc/stat
CPUStat leerCPU() {
  std::ifstream file("/proc/stat");

  if (!file.is_open()) {
    throw ErrorRecoleccion("No se pudo abrir /proc/stat");
  }

  std::string line;
  if (!std::getline(file, line)) {
    throw ErrorRecoleccion("No se pudo leer la línea principal de /proc/stat");
  }

  std::istringstream ss(line);

  std::string cpu;
  CPUStat stat;

  if (!(ss >> cpu >> stat.user >> stat.nice >> stat.system >> stat.idle >>
        stat.iowait >> stat.irq >> stat.softirq >> stat.steal)) {
    throw ErrorRecoleccion("Formato inesperado en /proc/stat");
  }

  return stat;
}

// Calcular uso CPU entre dos mediciones
double calcularUso(const CPUStat& a, const CPUStat& b) {
  const std::uint64_t idleA = a.idle + a.iowait;
  const std::uint64_t idleB = b.idle + b.iowait;

  const std::uint64_t totalA = a.user + a.nice + a.system + a.idle +
                               a.iowait + a.irq + a.softirq + a.steal;
  const std::uint64_t totalB = b.user + b.nice + b.system + b.idle +
                               b.iowait + b.irq + b.softirq + b.steal;

  if (totalB <= totalA || idleB < idleA) {
    return 0.0;
  }

  const std::uint64_t totalDelta = totalB - totalA;
  const std::uint64_t idleDelta = idleB - idleA;

  if (totalDelta == 0) {
    return 0.0;
  }

  return 100.0 *
         (1.0 - static_cast<double>(idleDelta) / static_cast<double>(totalDelta));
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
    if (line.size() > 3 && line.rfind("cpu", 0) == 0 &&
        std::isdigit(static_cast<unsigned char>(line[3]))) {
      ++cores;
    }
  }

  return cores;
}

}  // namespace

#endif  // defined(__linux__)

// Nombre del collector
std::string CollectorCPU::nombre() const { return "cpu"; }

// Función principal
std::vector<chuspapulse::core::Metrica> CollectorCPU::recolectar() {
#if defined(__linux__)
  const CPUStat a = leerCPU();

  std::this_thread::sleep_for(std::chrono::milliseconds(120));

  const CPUStat b = leerCPU();

  double usage = calcularUso(a, b);
  const int cores = contarCores();
  const auto timestamp = static_cast<std::int64_t>(std::time(nullptr));

  if (usage < 0.0) {
    usage = 0.0;
  }
  if (usage > 100.0) {
    usage = 100.0;
  }

  return {
      chuspapulse::core::Metrica{"cpu.usage", usage, "porcentaje", timestamp},
      chuspapulse::core::Metrica{"cpu.cores", static_cast<double>(cores),
                               "cantidad", timestamp}};
#else
  throw ErrorRecoleccion("CollectorCPU solo está soportado en Linux");
#endif  // defined(__linux__)
}

}  // namespace chuspapulse::collectors
