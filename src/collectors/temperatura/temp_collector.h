#pragma once

#include <string>

namespace collectors {

struct TempInfo {
    float       cpu_celsius;
    std::string zona;
    bool        disponible;
};

/**
 * Lee la temperatura de CPU desde /sys/class/thermal/thermal_zone0/temp.
 *
 * El kernel expone el valor como entero en milicélsius; esta función lo
 * divide entre 1000.0f para retornarlo en grados Celsius.
 *
 * Manejo de errores: nunca lanza excepción ni aborta el proceso. Si el archivo
 * no puede abrirse, si su contenido no puede leerse como entero o si la lectura
 * queda fuera del rango plausible (0 a 150 grados Celsius), retorna TempInfo
 * con disponible=false y cpu_celsius=0.0f. En cada uno de estos casos se
 * registra una advertencia descriptiva con el motivo y la ruta del archivo.
 *
 * El campo zona siempre contiene "thermal_zone0", incluso cuando la lectura
 * no está disponible.
 */
TempInfo getTempInfo();

} // namespace collectors