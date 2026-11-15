#ifdef _WIN32

#include "collector_cpu.hpp"
#include "../../collectors/error_recoleccion.hpp"

// Windows headers: WIN32_LEAN_AND_MEAN excluye headers de Win32 que no se
// necesitan (Winsock v1, macros de min/max, etc.) para reducir el tiempo
// de compilacion y evitar conflictos con la STL.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <chrono>
#include <ctime>
#include <thread>

namespace chuspapulse::platform::windows_platform {

namespace {

/**
 * @brief Convierte un FILETIME (entero de 64 bits en unidades de 100ns)
 *        a un valor uint64_t comparable directamente.
 *
 * FILETIME no puede usarse directamente en aritmetica: la documentacion
 * de Microsoft advierte que copiar los campos dwLowDateTime/dwHighDateTime
 * a un ULARGE_INTEGER es la forma correcta y portable de hacerlo.
 */
uint64_t filetimeAUint64(const FILETIME& ft) {
    ULARGE_INTEGER uli;
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return uli.QuadPart;
}

/**
 * @brief Snapshot de los tiempos de sistema obtenidos con GetSystemTimes.
 */
struct TiemposSistema {
    uint64_t idle;    ///< Tiempo en estado idle
    uint64_t kernel;  ///< Tiempo en modo kernel (incluye idle en Windows)
    uint64_t user;    ///< Tiempo en modo usuario
};

/**
 * @brief Llama a GetSystemTimes y retorna los valores como uint64_t.
 *
 * @throws chuspapulse::collectors::ErrorRecoleccion si la llamada falla.
 */
TiemposSistema leerTiemposSistema() {
    FILETIME idle_ft, kernel_ft, user_ft;

    if (!GetSystemTimes(&idle_ft, &kernel_ft, &user_ft)) {
        throw chuspapulse::collectors::ErrorRecoleccion(
            "GetSystemTimes fallo (codigo: " +
            std::to_string(static_cast<unsigned long>(GetLastError())) + ")"
        );
    }

    return TiemposSistema{
        filetimeAUint64(idle_ft),
        filetimeAUint64(kernel_ft),
        filetimeAUint64(user_ft)
    };
}

/**
 * @brief Calcula el porcentaje de uso de CPU entre dos muestras.
 *
 * En Windows, kernelTime incluye idleTime, por lo que el tiempo
 * activo total es (kernel + user - idle) y el tiempo total es
 * (kernel + user). La formula resulta:
 *
 *   uso% = 100 * (1 - delta_idle / (delta_kernel + delta_user))
 *
 * @param a Primera lectura.
 * @param b Segunda lectura (posterior a a).
 * @return Porcentaje de CPU en uso, en el rango [0.0, 100.0].
 */
double calcularUso(const TiemposSistema& a, const TiemposSistema& b) {
    const uint64_t delta_idle   = b.idle   - a.idle;
    const uint64_t delta_kernel = b.kernel - a.kernel;
    const uint64_t delta_user   = b.user   - a.user;

    // delta_kernel ya incluye delta_idle, por lo que el total es:
    const uint64_t delta_total = delta_kernel + delta_user;

    if (delta_total == 0) {
        return 0.0;
    }

    double uso = 100.0 * (1.0 - static_cast<double>(delta_idle) /
                                static_cast<double>(delta_total));

    if (uso < 0.0)   uso = 0.0;
    if (uso > 100.0) uso = 100.0;

    return uso;
}

/**
 * @brief Obtiene el numero de cores/procesadores logicos del sistema.
 *
 * Usa GetSystemInfo, disponible sin privilegios en todas las versiones
 * de Windows soportadas (10/11).
 */
int contarCores() {
    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    return static_cast<int>(si.dwNumberOfProcessors);
}

} // namespace

std::string CollectorCPUWindows::nombre() const {
    return "cpu";
}

std::vector<chuspapulse::core::Metrica> CollectorCPUWindows::recolectar() {
    // Primera lectura
    TiemposSistema a = leerTiemposSistema();

    // Intervalo identico al collector Linux (120 ms) para mantener
    // la misma granularidad de medicion entre plataformas.
    std::this_thread::sleep_for(std::chrono::milliseconds(120));

    // Segunda lectura
    TiemposSistema b = leerTiemposSistema();

    const double uso   = calcularUso(a, b);
    const int    cores = contarCores();
    const auto   ts    = static_cast<std::int64_t>(std::time(nullptr));

    return {
        chuspapulse::core::Metrica{"cpu.usage", uso,                        "porcentaje", ts},
        chuspapulse::core::Metrica{"cpu.cores", static_cast<double>(cores), "cantidad",   ts}
    };
}

} // namespace chuspapulse::platform::windows_platform

#endif // _WIN32
