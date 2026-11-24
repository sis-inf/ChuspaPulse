#ifndef LOADAVG_COLLECTOR_H
#define LOADAVG_COLLECTOR_H

/**
 * Estructura que almacena los promedios de carga del sistema
 * para los últimos 1, 5 y 15 minutos respectivamente.
 */
struct LoadAvgInfo {
    float load1;
    float load5;
    float load15;
};

/**
 * Lee el archivo virtual /proc/loadavg y extrae los promedios de carga.
 *
 * Los valores se devuelven redondeados a dos decimales.
 *
 * @return Una estructura LoadAvgInfo poblada con las métricas del sistema.
 * @throws chuspapulse::collectors::ErrorRecoleccion (derivada de
 *         std::runtime_error) con un mensaje descriptivo si:
 *         - /proc/loadavg no puede abrirse o no existe,
 *         - ocurre un error de lectura del archivo,
 *         - el formato es inválido o los datos están incompletos,
 *         - algún valor no es finito o es negativo.
 *
 * @note La función no aborta el proceso: el llamador debe capturar la
 *       excepción para tratar el fallo como un estado de error controlado.
 */
LoadAvgInfo getLoadAverage();

#endif // LOADAVG_COLLECTOR_H
