#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace chuspapulse::collectors::procesos {

/**
 * @brief Informacion resumida de un proceso para el ranking de "top procesos".
 *
 * Los valores de CPU son aproximados: se calculan a partir del delta de
 * utime+stime entre dos lecturas separadas por un intervalo corto, similar
 * a como herramientas como `top` o `ps` estiman el uso instantaneo de CPU.
 */
struct ProcesoInfo {
    /** @brief PID del proceso. */
    int32_t pid{};

    /** @brief Nombre del ejecutable (campo "comm" de /proc/[pid]/stat). */
    std::string nombre;

    /** @brief Uso aproximado de CPU en porcentaje, calculado como el
     *         delta de ticks (utime+stime) entre dos lecturas, normalizado
     *         por el numero de ticks del sistema y el intervalo transcurrido.
     *         Puede superar 100% en procesos multi-hilo en sistemas
     *         multi-core. */
    double cpu_porcentaje{};

    /** @brief Memoria residente (RSS) del proceso en bytes, leida desde
     *         VmRSS en /proc/[pid]/status. */
    uint64_t memoria_rss_bytes{};
};

/**
 * @brief Obtiene los N procesos con mayor uso combinado de CPU y memoria.
 *
 * Itera los directorios numericos de /proc (un directorio por PID activo),
 * toma una primera lectura de utime+stime desde /proc/[pid]/stat, espera
 * un intervalo corto, y toma una segunda lectura para calcular el delta de
 * uso de CPU. La memoria residente (VmRSS) se lee de /proc/[pid]/status en
 * la segunda pasada.
 *
 * El "uso combinado" usado para ordenar es una suma ponderada de:
 * - cpu_porcentaje (0-100, puede superar 100 en multi-core)
 * - memoria_rss_bytes normalizada a porcentaje de memoria total del sistema
 *
 * @param n Numero maximo de procesos a retornar (por defecto 5).
 * @param intervalo_ms Intervalo en milisegundos entre la primera y segunda
 *                      lectura de /proc/[pid]/stat, usado para calcular el
 *                      delta de CPU (por defecto 100 ms).
 * @return Vector con hasta @p n elementos ProcesoInfo, ordenados de mayor a
 *         menor uso combinado de CPU+memoria. Si /proc no esta disponible o
 *         no se puede leer ningun proceso, retorna un vector vacio sin
 *         lanzar excepcion.
 *
 * @note Los procesos que terminan entre la primera y la segunda lectura
 *       (o que desaparecen mientras se itera /proc) se omiten en silencio;
 *       esta es una condicion de carrera esperada al inspeccionar /proc y
 *       no se considera un error.
 * @note No requiere privilegios de root: solo se leen los archivos de
 *       /proc/[pid]/ a los que el usuario actual ya tiene acceso (sus
 *       propios procesos, o todos si se ejecuta con privilegios elevados).
 *       Los procesos de otros usuarios a los que no se tiene acceso de
 *       lectura simplemente se omiten.
 */
std::vector<ProcesoInfo> getTopProcesos(int n = 5, int intervalo_ms = 100);

} // namespace chuspapulse::collectors::procesos
