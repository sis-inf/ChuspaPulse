#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include "../../collectors/icollector.hpp"
#include "../../core/types.hpp"

namespace chuspapulse::platform::linux_platform {

/**
 * @brief Collector que mide el espacio de disco en el sistema de archivos raíz.
 *
 * Usa la llamada al sistema statvfs(3) sobre el punto de montaje "/".
 * Devuelve las métricas disk.total_bytes, disk.used_bytes y disk.free_bytes,
 * todas expresadas en bytes.
 *
 * El cálculo de espacio usado se obtiene como:
 *   disk.used_bytes = disk.total_bytes - disk.free_bytes
 *
 * donde disk.free_bytes usa f_bavail (bloques disponibles para usuarios no
 * privilegiados), de modo que la suma used + free puede ser menor o igual
 * que total (la diferencia corresponde a bloques reservados para root).
 *
 * @note Esta clase lanza std::runtime_error si statvfs falla, a diferencia
 *       de CollectorDisk que usa ErrorRecoleccion. Esto permite integrarla
 *       con capas que capturen std::exception de forma genérica.
 */
class CollectorDiskLinux : public chuspapulse::collectors::ICollector {
public:
    /// @brief Retorna el nombre identificador de este collector.
    /// @return "disk"
    std::string nombre() const override;

    /// @brief Recolecta las métricas de espacio de disco del sistema.
    /// @return Vector con disk.total_bytes, disk.used_bytes y disk.free_bytes.
    /// @throws std::runtime_error si statvfs("/", ...) falla.
    std::vector<chuspapulse::core::Metrica> recolectar() override;
};

} // namespace chuspapulse::platform::linux_platform