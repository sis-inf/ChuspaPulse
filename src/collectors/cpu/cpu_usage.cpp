##include "cpu_usage.hpp"
#include "../../collectors/error_recoleccion.hpp"
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <ctime>

/// @brief Lee y parsea los contadores de CPU desde /proc/stat.
/// @param ruta Ruta al archivo de estadísticas.
/// @return ContadoresCPU con los valores leídos.
/// @throws chuspapulse::collectors::ErrorRecoleccion si el archivo no es accesible.
ContadoresCPU LeerEstadisticasCPU(const std::string& ruta) {
  ContadoresCPU contadores{};
  std::ifstream archivo(ruta);
  if (!archivo.is_open()) {
    throw chuspapulse::collectors::ErrorRecoleccion(
        "No se puede acceder a " + ruta
    );
  }
  archivo.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    std::string linea;
    std::getline(archivo, linea);
    std::istringstream flujo(linea);
    std::string etiqueta;
    flujo >> etiqueta;
    if (etiqueta != "cpu") {
      throw chuspapulse::collectors::ErrorRecoleccion(
          "Formato inesperado en " + ruta
      );
    }
    flujo >> contadores.usuario
          >> contadores.nice
          >> contadores.sistema
          >> contadores.ocioso
          >> contadores.espera_io
          >> contadores.irq
          >> contadores.softirq
          >> contadores.robado;
  } catch (const chuspapulse::collectors::ErrorRecoleccion&) {
    throw;
  } catch (...) {
    throw chuspapulse::collectors::ErrorRecoleccion(
        "Error al leer " + ruta
    );
  }
  return contadores;
}

/// @brief Calcula la suma total de ticks de CPU.
/// @param c Contadores de CPU.
/// @return Total de ticks acumulados.
uint64_t CalcularTicksTotales(const ContadoresCPU& c) {
  return c.usuario + c.nice + c.sistema + c.ocioso +
         c.espera_io + c.irq + c.softirq + c.robado;
}

/// @brief Calcula los ticks correspondientes al tiempo ocioso.
/// @param c Contadores de CPU.
/// @return Ticks ociosos (idle + iowait).
uint64_t CalcularTicksOciosos(const ContadoresCPU& c) {
  return c.ocioso + c.espera_io;
}

/// @brief Calcula el uso de CPU en porcentaje.
/// Se basa en dos lecturas de /proc/stat separadas por 100 milisegundos.
/// @return Valor entre 0.0 y 100.0 que representa el uso de CPU.
double ObtenerUsoCPU() {
  // Primera lectura
  ContadoresCPU lectura1 = LeerEstadisticasCPU();
  uint64_t total1 = CalcularTicksTotales(lectura1);
  uint64_t ocioso1 = CalcularTicksOciosos(lectura1);

  // Esperar 100 milisegundos
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Segunda lectura
  ContadoresCPU lectura2 = LeerEstadisticasCPU();
  uint64_t total2 = CalcularTicksTotales(lectura2);
  uint64_t ocioso2 = CalcularTicksOciosos(lectura2);

  // Validar posibles inconsistencias
  if (total2 <= total1 || ocioso2 < ocioso1) {
    return 0.0;
  }

  uint64_t delta_total  = total2 - total1;
  uint64_t delta_ocioso = ocioso2 - ocioso1;

  if (delta_total == 0) {
    return 0.0;
  }

  double uso = (double)(delta_total - delta_ocioso) / delta_total * 100.0;

  if (uso < 0.0)   uso = 0.0;
  if (uso > 100.0) uso = 100.0;

  return uso;
}

/// @brief Obtiene el número de núcleos de CPU del sistema.
/// @return Número de núcleos lógicos disponibles.
static int ObtenerNucleosCPU() {
  int nucleos = 0;
  std::ifstream archivo("/proc/stat");
  if (!archivo.is_open()) {
    return 0;
  }
  std::string linea;
  while (std::getline(archivo, linea)) {
    if (linea.rfind("cpu", 0) == 0 && linea.size() > 3 &&
        std::isdigit(linea[3])) {
      nucleos++;
    }
  }
  return nucleos;
}

// Implementación de CollectorCPU
namespace chuspapulse::collectors {

std::string CollectorCPU::nombre() const {
  return "cpu";
}

std::vector<chuspapulse::core::Metrica> CollectorCPU::recolectar() {
  std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

  chuspapulse::core::Metrica uso;
  uso.name      = "cpu.usage";
  uso.value     = ObtenerUsoCPU();
  uso.unit      = "porcentaje";
  uso.timestamp = ahora;

  chuspapulse::core::Metrica nucleos;
  nucleos.name      = "cpu.cores";
  nucleos.value     = static_cast<double>(ObtenerNucleosCPU());
  nucleos.unit      = "nucleos";
  nucleos.timestamp = ahora;

  return { uso, nucleos };
}

}  // namespace chuspapulse::collectors
