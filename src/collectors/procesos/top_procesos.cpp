#include "top_procesos.hpp"

#include "../memory/ram_usage.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>

namespace chuspapulse::collectors::procesos {

namespace {

/// @brief Ticks de CPU por segundo configurados en el kernel (HZ).
/// sysconf(_SC_CLK_TCK) es la forma portable de obtenerlo en lugar de
/// asumir 100, que es el valor usual pero no garantizado en todas las
/// arquitecturas.
long ticksPorSegundo() {
    long valor = sysconf(_SC_CLK_TCK);
    return (valor > 0) ? valor : 100;
}

/// @brief Contadores de tiempo de CPU de un proceso individual, leidos
///        desde /proc/[pid]/stat.
struct StatProceso {
    int32_t pid{};
    std::string nombre;
    uint64_t utime{};
    uint64_t stime{};
};

/// @brief Lee y parsea /proc/[pid]/stat para un PID dado.
///
/// El campo "comm" (nombre del ejecutable) viene entre parentesis y puede
/// contener espacios o parentesis internos, por lo que se busca el ULTIMO
/// ')' en la linea para delimitarlo correctamente antes de parsear los
/// campos numericos que le siguen.
///
/// @param pid PID del proceso a leer.
/// @param out Estructura de salida con los valores leidos.
/// @return true si la lectura y el parseo fueron exitosos, false si el
///         proceso ya no existe o el archivo no tiene el formato esperado
///         (se trata como un caso esperado, no como un error fatal).
bool leerStatProceso(int32_t pid, StatProceso& out) {
    const std::string ruta = "/proc/" + std::to_string(pid) + "/stat";
    std::ifstream archivo(ruta);

    // El proceso pudo haber terminado entre el listado del directorio y
    // esta lectura: no es un error, simplemente se omite.
    if (!archivo.is_open()) {
        return false;
    }

    std::string linea;
    if (!std::getline(archivo, linea)) {
        return false;
    }

    const std::size_t inicio_nombre = linea.find('(');
    const std::size_t fin_nombre    = linea.rfind(')');

    if (inicio_nombre == std::string::npos ||
        fin_nombre == std::string::npos ||
        fin_nombre <= inicio_nombre) {
        return false;
    }

    out.pid    = pid;
    out.nombre = linea.substr(inicio_nombre + 1, fin_nombre - inicio_nombre - 1);

    // Despues del nombre vienen los campos separados por espacio:
    // state(3) ppid(4) ... utime(14) stime(15) ...
    // Como ya consumimos pid y comm, el resto empieza en el campo "state".
    std::istringstream resto(linea.substr(fin_nombre + 1));

    std::string campo;
    // Campo 3: state
    if (!(resto >> campo)) return false;

    // Campos 4 a 13 (10 campos) se descartan: ppid, pgrp, session, tty_nr,
    // tpgid, flags, minflt, cminflt, majflt, cmajflt.
    for (int i = 0; i < 10; ++i) {
        if (!(resto >> campo)) return false;
    }

    // Campo 14: utime, campo 15: stime
    if (!(resto >> out.utime)) return false;
    if (!(resto >> out.stime)) return false;

    return true;
}

/// @brief Lee VmRSS desde /proc/[pid]/status, en bytes.
///
/// @param pid PID del proceso a leer.
/// @param out_rss_bytes Salida con el valor de VmRSS convertido a bytes.
/// @return true si se encontro y parseo VmRSS correctamente, false si el
///         proceso ya no existe o el campo no esta presente.
bool leerMemoriaProceso(int32_t pid, uint64_t& out_rss_bytes) {
    const std::string ruta = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream archivo(ruta);

    if (!archivo.is_open()) {
        return false;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.rfind("VmRSS:", 0) == 0) {
            std::istringstream iss(linea.substr(6));
            uint64_t valor_kb = 0;
            if (!(iss >> valor_kb)) {
                return false;
            }
            out_rss_bytes = valor_kb * 1024ULL;
            return true;
        }
    }

    // Algunos procesos (p. ej. kernel threads o procesos a punto de morir)
    // pueden no tener VmRSS; se trata como 0 en lugar de error.
    out_rss_bytes = 0;
    return true;
}

/// @brief Lista los PIDs activos iterando los directorios numericos de
///        /proc.
///
/// @return Vector de PIDs encontrados. Vector vacio si /proc no puede
///         abrirse (no deberia ocurrir en un sistema Linux normal, pero
///         se maneja con gracia en lugar de lanzar excepcion).
std::vector<int32_t> listarPids() {
    std::vector<int32_t> pids;

    DIR* dir = opendir("/proc");
    if (dir == nullptr) {
        return pids;
    }

    struct dirent* entrada;
    while ((entrada = readdir(dir)) != nullptr) {
        const std::string nombre = entrada->d_name;

        if (nombre.empty() ||
            !std::all_of(nombre.begin(), nombre.end(),
                         [](unsigned char c) { return std::isdigit(c); })) {
            continue;
        }

        try {
            pids.push_back(static_cast<int32_t>(std::stol(nombre)));
        } catch (const std::exception&) {
            // PID con formato inesperado (no deberia pasar tras el filtro
            // de isdigit, pero se ignora por seguridad en vez de fallar).
            continue;
        }
    }

    closedir(dir);
    return pids;
}

} // namespace

std::vector<ProcesoInfo> getTopProcesos(int n, int intervalo_ms) {
    if (n <= 0) {
        return {};
    }

    const long hz = ticksPorSegundo();

    // -------------------------------------------------------------------
    // Primera lectura
    // -------------------------------------------------------------------
    std::vector<int32_t> pids = listarPids();

    std::unordered_map<int32_t, StatProceso> primera_lectura;
    primera_lectura.reserve(pids.size());

    for (int32_t pid : pids) {
        StatProceso stat;
        // Si el proceso ya no existe (termino justo despues del listado
        // del directorio), simplemente se omite: no es un error.
        if (leerStatProceso(pid, stat)) {
            primera_lectura.emplace(pid, std::move(stat));
        }
    }

    // -------------------------------------------------------------------
    // Esperar el intervalo configurado para poder calcular el delta
    // -------------------------------------------------------------------
    std::this_thread::sleep_for(std::chrono::milliseconds(intervalo_ms));

    // -------------------------------------------------------------------
    // Segunda lectura: reutilizamos la lista de PIDs original, pero
    // toleramos que algunos hayan desaparecido entre ambas lecturas.
    // -------------------------------------------------------------------
    std::vector<ProcesoInfo> resultado;
    resultado.reserve(primera_lectura.size());

    // Memoria total del sistema, usada para normalizar RSS a porcentaje.
    // Si no se puede leer, se usa 1 para evitar division por cero; en ese
    // caso memoria_pct quedara en 0 para todos los procesos, pero el CPU
    // seguira siendo valido.
    uint64_t memoria_total_bytes = 1;
    try {
        memoria_total_bytes = chuspapulse::collectors::memory::getRamUsage().total;
        if (memoria_total_bytes == 0) {
            memoria_total_bytes = 1;
        }
    } catch (const std::exception&) {
        memoria_total_bytes = 1;
    }

    // Intervalo real transcurrido en segundos, para normalizar el delta de
    // ticks de CPU a un porcentaje independiente del intervalo elegido.
    const double intervalo_segundos =
        static_cast<double>(intervalo_ms) / 1000.0;

    for (int32_t pid : pids) {
        auto it_previo = primera_lectura.find(pid);
        if (it_previo == primera_lectura.end()) {
            // No se pudo leer en la primera pasada: se omite.
            continue;
        }

        StatProceso segunda;
        // El proceso pudo haber terminado entre la primera y la segunda
        // lectura (condicion de carrera esperada al iterar /proc): se
        // omite en silencio en lugar de fallar.
        if (!leerStatProceso(pid, segunda)) {
            continue;
        }

        const StatProceso& primera = it_previo->second;

        // Los contadores de ticks son monotonicos crecientes; si el delta
        // es negativo (p. ej. el PID fue reciclado por otro proceso entre
        // lecturas), se descarta esa muestra en lugar de reportar un valor
        // negativo enganoso.
        if (segunda.utime < primera.utime || segunda.stime < primera.stime) {
            continue;
        }

        const uint64_t delta_ticks =
            (segunda.utime - primera.utime) + (segunda.stime - primera.stime);

        double cpu_pct = 0.0;
        if (intervalo_segundos > 0.0 && hz > 0) {
            cpu_pct = (static_cast<double>(delta_ticks) / static_cast<double>(hz))
                      / intervalo_segundos * 100.0;
        }

        uint64_t memoria_rss_bytes = 0;
        // Si el proceso desaparecio justo antes de leer su memoria, se usa
        // 0 en lugar de descartar la muestra de CPU ya calculada.
        leerMemoriaProceso(pid, memoria_rss_bytes);

        ProcesoInfo info;
        info.pid               = pid;
        info.nombre            = segunda.nombre;
        info.cpu_porcentaje    = cpu_pct;
        info.memoria_rss_bytes = memoria_rss_bytes;

        resultado.push_back(std::move(info));
    }

    // -------------------------------------------------------------------
    // Ordenar por uso combinado de CPU + memoria (normalizada a
    // porcentaje del total del sistema) y devolver los primeros N.
    // -------------------------------------------------------------------
    auto usoCombinado = [memoria_total_bytes](const ProcesoInfo& p) {
        const double memoria_pct =
            (static_cast<double>(p.memoria_rss_bytes) /
             static_cast<double>(memoria_total_bytes)) * 100.0;
        return p.cpu_porcentaje + memoria_pct;
    };

    std::sort(resultado.begin(), resultado.end(),
              [&usoCombinado](const ProcesoInfo& a, const ProcesoInfo& b) {
                  return usoCombinado(a) > usoCombinado(b);
              });

    if (static_cast<int>(resultado.size()) > n) {
        resultado.resize(static_cast<std::size_t>(n));
    }

    return resultado;
}

} // namespace chuspapulse::collectors::procesos
