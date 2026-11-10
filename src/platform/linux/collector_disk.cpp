#include "collector_disk.hpp"

#include <sys/statvfs.h>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <stdexcept>

namespace chuspapulse::platform::linux_platform {

std::string CollectorDiskLinux::nombre() const {
    return "disk";
}

std::vector<chuspapulse::core::Metrica> CollectorDiskLinux::recolectar() {
    struct statvfs buf{};

    // statvfs(3) sobre el punto de montaje raíz.
    // Devuelve 0 en éxito; en fallo devuelve -1 y setea errno.
    if (statvfs("/", &buf) != 0) {
        throw std::runtime_error(
            std::string("statvfs(\"/\") falló: ") + std::strerror(errno)
        );
    }

    // f_frsize: tamaño fundamental del bloque del sistema de archivos (bytes).
    // f_blocks: total de bloques en el sistema de archivos.
    // f_bavail: bloques disponibles para usuarios no privilegiados.
    // f_bfree : bloques libres totales (incluye reservados para root).
    //
    // Se usa f_bavail para disk.free_bytes porque es el espacio real
    // disponible para procesos de usuario, consistente con lo que reporta df(1).
    const double total_bytes =
        static_cast<double>(buf.f_blocks) * static_cast<double>(buf.f_frsize);

    const double free_bytes =
        static_cast<double>(buf.f_bavail) * static_cast<double>(buf.f_frsize);

    const double used_bytes = total_bytes - free_bytes;

    const std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    return {
        {"disk.total_bytes", total_bytes, "bytes", ahora},
        {"disk.used_bytes",  used_bytes,  "bytes", ahora},
        {"disk.free_bytes",  free_bytes,  "bytes", ahora},
    };
}

} // namespace chuspapulse::platform::linux_platform
