#include "disk_usage.h"
#include <sys/statvfs.h>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <cstdint>
#include <string>
#include <ctime>

/// @brief Estructura que representa el uso de disco del sistema.
/// Todos los valores están expresados en bytes.
struct DiskUsage {
    uint64_t total{}; ///< Espacio total del sistema de archivos
    uint64_t usado{}; ///< Espacio actualmente utilizado
    uint64_t libre{}; ///< Espacio disponible para el usuario
};

/// @brief Obtiene las métricas de uso de disco usando statvfs.
/// Calcula: total = buf.f_blocks * buf.f_frsize,
///          free  = buf.f_bavail * buf.f_frsize,
///          used  = total - free.
/// @param ruta Ruta del punto de montaje a consultar (por defecto "/").
/// @return Estructura DiskUsage con total, usado y libre en bytes.
/// @throws std::runtime_error si statvfs falla, con el errno correspondiente.
DiskUsage getDiskUsage(const std::string& ruta = "/") {
    struct statvfs buf;

    if (statvfs(ruta.c_str(), &buf) != 0) {
        throw std::runtime_error(
            std::string("Error al leer métricas de disco: ") + std::strerror(errno)
        );
    }

    DiskUsage uso{};
    uso.total = static_cast<uint64_t>(buf.f_blocks) * buf.f_frsize;
    uso.libre  = static_cast<uint64_t>(buf.f_bavail) * buf.f_frsize;
    uso.usado  = uso.total - uso.libre;

    return uso;
}

namespace pulso::collectors::disk {

std::string DiskCollector::nombre() const {
    return "disk";
}

std::vector<pulso::core::Metrica> DiskCollector::recolectar() {
    const DiskUsage uso = getDiskUsage();
    const std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    return {
        {"disk.total", static_cast<double>(uso.total), "bytes", ahora},
        {"disk.used", static_cast<double>(uso.usado), "bytes", ahora},
        {"disk.free", static_cast<double>(uso.libre), "bytes", ahora},
    };
}

}  // namespace pulso::collectors::disk
