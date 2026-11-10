#ifndef CHUSPAPULSE_CPU_CPU_USAGE_HPP
#define CHUSPAPULSE_CPU_CPU_USAGE_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "../../collectors/icollector.hpp"

/// @brief Estructura que representa los contadores de tiempo de CPU
/// obtenidos desde la primera línea de /proc/stat.
/// Cada campo representa tiempo acumulado en distintos estados.
struct ContadoresCPU {
  uint64_t usuario{};     ///< Tiempo en modo usuario
  uint64_t nice{};        ///< Tiempo en modo usuario con prioridad ajustada
  uint64_t sistema{};     ///< Tiempo en modo kernel (sistema)
  uint64_t ocioso{};      ///< Tiempo en estado idle (sin uso)
  uint64_t espera_io{};   ///< Tiempo esperando operaciones de entrada/salida (I/O)
  uint64_t irq{};         ///< Tiempo atendiendo interrupciones de hardware
  uint64_t softirq{};     ///< Tiempo atendiendo interrupciones de software
  uint64_t robado{};      ///< Tiempo "robado" por otras máquinas virtuales
};

/// @brief Lee y parsea la primera línea del archivo /proc/stat.
/// @param ruta Ruta al archivo (por defecto "/proc/stat").
/// @return Estructura ContadoresCPU con los valores leídos.
/// @note En caso de error, retorna todos los valores en cero.
ContadoresCPU LeerEstadisticasCPU(const std::string& ruta = "/proc/stat");

/// @brief Calcula la suma total de ticks de CPU.
/// @param c Estructura con contadores de CPU.
/// @return Total de ticks acumulados.
uint64_t CalcularTicksTotales(const ContadoresCPU& c);

/// @brief Calcula los ticks correspondientes al tiempo ocioso.
/// @param c Estructura con contadores de CPU.
/// @return Ticks ociosos (idle + iowait).
uint64_t CalcularTicksOciosos(const ContadoresCPU& c);

/// @brief Calcula el uso de CPU en porcentaje (0.0 - 100.0).
/// Realiza dos lecturas de /proc/stat con un intervalo de 1 segundo.
/// @return Porcentaje de uso de CPU.
/// @note Si ocurre un error o los datos no son válidos, retorna 0.0.
double ObtenerUsoCPU();
namespace chuspapulse::collectors {

/**
 * @brief Recolector de métricas de CPU.
 *
 * Implementa la interfaz ICollector para ser compatible con el sistema
 * de colección del proyecto. Obtiene el porcentaje de uso de CPU
 * leyendo /proc/stat mediante la lógica ya existente en ObtenerUsoCPU().
 */
class CollectorCPU : public ICollector {
 public:
  /**
   * @brief Devuelve el nombre identificador de este collector.
   * @return La cadena "cpu".
   */
  std::string nombre() const override;

  /**
   * @brief Ejecuta una medición de CPU y devuelve las métricas obtenidas.
   *
   * Invoca ObtenerUsoCPU() para leer /proc/stat y calcula el porcentaje
   * de uso. Devuelve una métrica con nombre "cpu.usage" y unidad "porcentaje".
   *
   * @return Vector con la métrica de uso de CPU.
   */
  std::vector<chuspapulse::core::Metrica> recolectar() override;
};

}  // namespace chuspapulse::collectors
#endif // CHUSPAPULSE_CPU_CPU_USAGE_HPP
