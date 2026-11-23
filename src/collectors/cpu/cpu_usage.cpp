#include "cpu_usage.hpp"
#include "../../collectors/error_recoleccion.hpp"

#include <chrono>
#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

/// @brief Lee y parsea los contadores de CPU desde /proc/stat.
/// @param ruta Ruta al archivo de estadísticas.
/// @return ContadoresCPU con los valores leídos.
/// @throws chuspapulse::collectors::ErrorRecoleccion si el archivo no es accesible o su formato es inválido.
ContadoresCPU LeerEstadisticasCPU(const std::string& ruta) {
  if (ruta.empty()) {
    const std::string msg = "La ruta del archivo de estadisticas de CPU esta vacia.";
    std::cerr << "[cpu_usage] Error de lectura: " << msg << "\n";
    throw chuspapulse::collectors::ErrorRecoleccion(msg);
  }

  ContadoresCPU contadores{};
  std::ifstream archivo(ruta);
  if (!archivo.is_open()) {
    const std::string msg = "No se puede acceder a " + ruta;
    std::cerr << "[cpu_usage] Fallo de apertura: " << msg << "\n";
    throw chuspapulse::collectors::ErrorRecoleccion(msg);
  }

  try {
    std::string linea;
    if (!std::getline(archivo, linea) || linea.empty()) {
      const std::string msg = "El archivo " + ruta + " esta vacio o no contiene lineas legibles.";
      std::cerr << "[cpu_usage] Fallo de lectura: " << msg << "\n";
      throw chuspapulse::collectors::ErrorRecoleccion(msg);
    }

    std::istringstream flujo(linea);
    std::string etiqueta;
    if (!(flujo >> etiqueta) || etiqueta != "cpu") {
      const std::string msg = "Formato inesperado en " + ruta + " (se esperaba etiqueta 'cpu').";
      std::cerr << "[cpu_usage] Error de parseo: " << msg << "\n";
      throw chuspapulse::collectors::ErrorRecoleccion(msg);
    }

    if (!(flujo >> contadores.usuario
                >> contadores.nice
                >> contadores.sistema
                >> contadores.ocioso
                >> contadores.espera_io
                >> contadores.irq
                >> contadores.softirq
                >> contadores.robado)) {
      const std::string msg = "Campos numericos incompletos o corruptos en " + ruta;
      std::cerr << "[cpu_usage] Error de parseo: " << msg << "\n";
      throw chuspapulse::collectors::ErrorRecoleccion(msg);
    }
  } catch (const chuspapulse::collectors::ErrorRecoleccion&) {
    throw;
  } catch (const std::exception& ex) {
    const std::string msg = "Error al leer " + ruta + ": " + ex.what();
    std::cerr << "[cpu_usage] Excepcion durante lectura: " << msg << "\n";
    throw chuspapulse::collectors::ErrorRecoleccion(msg);
  } catch (...) {
    const std::string msg = "Error desconocido al leer " + ruta;
    std::cerr << "[cpu_usage] Error desconocido durante lectura: " << msg << "\n";
    throw chuspapulse::collectors::ErrorRecoleccion(msg);
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
/// @return Valor entre 0.0 y 100.0 que representa el uso de CPU, o 0.0 ante un fallo controlado.
double ObtenerUsoCPU() {
  try {
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
      std::cerr << "[cpu_usage] Advertencia: contadores de CPU inconsistentes entre lecturas (total1="
                << total1 << ", total2=" << total2 << "). Retornando 0.0.\n";
      return 0.0;
    }

    uint64_t delta_total  = total2 - total1;
    uint64_t delta_ocioso = ocioso2 - ocioso1;

    if (delta_total == 0) {
      return 0.0;
    }

    double uso = static_cast<double>(delta_total - delta_ocioso) / static_cast<double>(delta_total) * 100.0;

    if (uso < 0.0)   uso = 0.0;
    if (uso > 100.0) uso = 100.0;

    return uso;
  } catch (const std::exception& ex) {
    std::cerr << "[cpu_usage] Fallo controlado al obtener uso de CPU: " << ex.what()
              << ". Retornando estado seguro 0.0.\n";
    return 0.0;
  } catch (...) {
    std::cerr << "[cpu_usage] Error desconocido controlado al obtener uso de CPU. Retornando estado seguro 0.0.\n";
    return 0.0;
  }
}
