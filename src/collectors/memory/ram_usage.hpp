#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../icollector.hpp"
#include "core/types.hpp"

namespace chuspapulse::collectors::memory {

/**
 * @brief Contiene la informacion de uso de memoria RAM del sistema.
 */
struct RamInfo {
    /** @brief Memoria total disponible en el sistema, expresada en bytes. */
    uint64_t total;

    /** @brief Memoria actualmente en uso, expresada en bytes. */
    uint64_t used;

    /** @brief Memoria disponible para ser utilizada, expresada en bytes. */
    uint64_t available;
};

/**
 * @brief Obtiene las metricas actuales de uso de memoria RAM del sistema.
 * @param path Ruta al archivo de información de memoria (por defecto "/proc/meminfo").
 *             Permite inyectar una ruta alternativa en tests sin modificar el código de producción.
 * @return RamInfo con los valores de memoria total, usada y disponible.
 * @throws chuspapulse::collectors::ErrorRecoleccion si el archivo no se puede abrir,
 *         falla su lectura, MemTotal o MemAvailable faltan o tienen un valor inválido,
 *         MemAvailable supera a MemTotal o la conversión a bytes desborda.
 * @note Ante un fallo no aborta el proceso: registra un mensaje descriptivo en
 *       std::cerr y lanza la excepción para que el llamador la maneje.
 */
RamInfo getRamUsage(const std::string& path = "/proc/meminfo");

/**
 * @brief Collector de métricas de memoria RAM.
 */
class CollectorMemory : public chuspapulse::collectors::ICollector {
public:
    /**
     * @brief Devuelve el nombre identificador de este collector.
     * @return La cadena "memory".
     */
    std::string nombre() const override;

    /**
     * @brief Ejecuta una medición de memoria y devuelve las métricas obtenidas.
     * @return Vector con la métrica "memory.used" en bytes.
     * @throws chuspapulse::collectors::ErrorRecoleccion si falla la lectura de
     *         /proc/meminfo (ver getRamUsage). El llamador debe capturarla para
     *         que el fallo no aborte el proceso.
     */
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

} // namespace chuspapulse::collectors::memory
