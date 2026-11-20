#include "proc_collector.h"
#include "../error_recoleccion.hpp"
#include "utils/logging/logger.hpp"

#include <charconv>
#include <string>
#include <fstream>
#include <sstream>
#include <system_error>

namespace chuspapulse::collectors {

namespace {

[[noreturn]] void registrarError(const std::string& mensaje) {
    const std::string detalle = "proc_collector: " + mensaje;

    chuspapulse::utils::logging::Logger::instancia().warn(detalle);

    throw ErrorRecoleccion(detalle);
}

uint32_t convertirValor(const std::string& texto,
                        const std::string& contexto) {
    uint32_t valor = 0;

    const auto resultado = std::from_chars(
        texto.data(),
        texto.data() + texto.size(),
        valor
    );

    if (resultado.ec != std::errc{} ||
        resultado.ptr != texto.data() + texto.size()) {
        registrarError("Valor invalido en " + contexto + ": " + texto);
    }

    return valor;
}

} // namespace

ProcInfo getProcInfo() {
    ProcInfo info{};

    // =========================
    // Leer /proc/stat
    // =========================
    std::ifstream statFile("/proc/stat");

    if (!statFile.is_open()) {
        registrarError(
            "No se pudo abrir /proc/stat para leer informacion de procesos."
        );
    }

    std::string line;
    bool runningEncontrado = false;
    bool blockedEncontrado = false;

    while (std::getline(statFile, line)) {
        std::istringstream iss(line);
        std::string key;
        std::string value;

        if (!(iss >> key)) {
            continue;
        }

        if (key != "procs_running" && key != "procs_blocked") {
            continue;
        }

        if (!(iss >> value)) {
            registrarError(
                "No se pudo leer " + key + " desde /proc/stat."
            );
        }

        const uint32_t valor = convertirValor(
            value, "/proc/stat: " + key
        );

        if (key == "procs_running") {
            info.running = valor;
            runningEncontrado = true;
        } else {
            info.blocked = valor;
            blockedEncontrado = true;
        }
    }

    if (statFile.bad() || !statFile.eof()) {
        registrarError("Error durante la lectura de /proc/stat.");
    }

    if (!runningEncontrado || !blockedEncontrado) {
        registrarError(
            "Claves procs_running o procs_blocked ausentes en /proc/stat."
        );
    }

    statFile.close();

    // =========================
    // Leer /proc/loadavg
    // =========================
    std::ifstream loadavgFile("/proc/loadavg");

    if (!loadavgFile.is_open()) {
        registrarError(
            "No se pudo abrir /proc/loadavg para leer el total de procesos."
        );
    }

    std::string load1;
    std::string load5;
    std::string load15;
    std::string procesos;

    if (!(loadavgFile >> load1 >> load5 >> load15 >> procesos)) {
        registrarError("Lectura incompleta o fallida de /proc/loadavg.");
    }

    loadavgFile.close();

    const std::size_t separator = procesos.find('/');

    if (separator == std::string::npos ||
        separator == 0 ||
        separator == procesos.size() - 1) {
        registrarError(
            "Formato invalido en /proc/loadavg: se esperaba activos/total."
        );
    }

    convertirValor(
        procesos.substr(0, separator),
        "/proc/loadavg: procesos activos"
    );

    const std::string totalStr = procesos.substr(separator + 1);

    info.total = convertirValor(
        totalStr, "/proc/loadavg: total de procesos"
    );

    if (info.running > info.total) {
        info.running = info.total;
    }

    return info;
}

} // namespace chuspapulse::collectors
