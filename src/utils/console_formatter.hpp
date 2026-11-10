#pragma once

#include <map>
#include <string>

#include "core/types.hpp"

/**
 * @brief Formatea un snapshot de métricas del sistema como tabla ASCII.
 *
 * Genera una tabla de ancho fijo (60 caracteres) adecuada para imprimirse
 * directamente en una terminal. La tabla incluye cuatro filas de métricas:
 * CPU, RAM, Disco y Red. Las filas de CPU y RAM llevan una barra de progreso
 * ASCII que refleja visualmente el porcentaje de uso real.
 *
 * Las métricas se extraen del snapshot por nombre canónico. Los nombres
 * aceptados para cada campo son:
 *
 * | Campo      | Nombres aceptados en Metrica::nombre          |
 * |------------|-----------------------------------------------|
 * | CPU        | `"cpu.usage"`, `"cpu"`                        |
 * | RAM usada  | `"ram.used"`, `"ram.usado"`                   |
 * | RAM total  | `"ram.total"`                                 |
 * | Disco used | `"disk.used"`, `"disco.usado"`                |
 * | Disco tot. | `"disk.total"`, `"disco.total"`               |
 * | Red RX     | `"network.rx"`, `"net.rx"`                    |
 * | Red TX     | `"network.tx"`, `"net.tx"`                    |
 *
 * Ejemplo de salida:
 * @code
 * ============================================================
 * | Métrica      |             Valor | Uso                 |
 * |---------------+-------------------+---------------------|
 * | CPU           |            45.3 % | [########----------]|
 * | RAM           |      3.2 / 8.0 GB | [#######-----------]|
 * | Disco         |  120.0 / 500.0 GB | [####--------------]|
 * | Red  ↓↑       |    1.2 / 0.8 KB/s |                     |
 * ============================================================
 * @endcode
 *
 * @param snap  Snapshot de métricas capturado por el agente ChuspaPulse.
 * @return      String con la tabla ASCII completa, lista para imprimir con
 *              `std::cout`.
 *
 * @note Los valores de RAM, disco y red se expresan en GB y KB/s
 *       respectivamente. Si un valor total es 0, el porcentaje de la barra
 *       se muestra como 0%.
 *
 * @see chuspapulse::core::Snapshot
 * @see chuspapulse::core::Metrica
 */
std::string formatAsTable(const chuspapulse::core::Snapshot& snap);

/**
 * @brief Imprime métricas clave-valor directamente en `std::cout` (legacy).
 *
 * Recorre el mapa e imprime cada entrada en una línea con el formato:
 * @code
 * NOMBRE:   VALOR%
 * @endcode
 * Las entradas con valor negativo se omiten.
 *
 * @param data  Mapa de nombre de métrica → valor numérico (porcentaje).
 *
 * @deprecated Usar formatAsTable() para una presentación más completa.
 *             Esta función se mantiene por compatibilidad con el test
 *             legacy ConsoleFormatterTest.
 */
void printMetrics(const std::map<std::string, double>& data);
