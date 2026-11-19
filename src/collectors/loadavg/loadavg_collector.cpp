#include "loadavg_collector.h"
#include <fstream>
#include "../error_recoleccion.hpp"
#include <cmath>

// Función auxiliar para asegurar precisión de dos decimales
static float redondearDosDecimales(float val) {
    return std::round(val * 100.0f) / 100.0f;
}

LoadAvgInfo getLoadAverage() {
    // Abrir el archivo virtual del sistema operativo Linux
    std::ifstream file("/proc/loadavg");

    // Controlar errores de apertura
    if (!file.is_open()) {
        throw chuspapulse::collectors::ErrorRecoleccion(
            "No se pudo abrir el archivo /proc/loadavg");
    }

    LoadAvgInfo info{0.0f, 0.0f, 0.0f};

    // Leer y parsear los primeros 3 valores float separados por espacios en blanco
    if (!(file >> info.load1 >> info.load5 >> info.load15)) {
        if (file.bad()) {
            throw chuspapulse::collectors::ErrorRecoleccion(
                "Error de lectura en el archivo /proc/loadavg");
        }

        throw chuspapulse::collectors::ErrorRecoleccion(
            "Formato invalido o datos incompletos en /proc/loadavg");
    }

    // Validar los valores obtenidos
    if (!std::isfinite(info.load1) ||
        !std::isfinite(info.load5) ||
        !std::isfinite(info.load15) ||
        info.load1 < 0.0f ||
        info.load5 < 0.0f ||
        info.load15 < 0.0f) {
        throw chuspapulse::collectors::ErrorRecoleccion(
            "Valores de carga invalidos en /proc/loadavg");
    }

    // Normalizar asegurando precisión de dos decimales
    info.load1 = redondearDosDecimales(info.load1);
    info.load5 = redondearDosDecimales(info.load5);
    info.load15 = redondearDosDecimales(info.load15);

    return info;
}
