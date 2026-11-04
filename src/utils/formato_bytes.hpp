#pragma once

#include <string>

namespace chuspapulse::utils {

/**
 * @brief Formatea un valor en bytes a una representacion legible para
 *        humanos, seleccionando automaticamente la unidad mas apropiada.
 *
 * Similar al comportamiento de comandos Unix como `du -h` o `ls -lh`:
 * el valor se divide sucesivamente entre 1024 hasta quedar en el rango
 * [1, 1024), eligiendo la unidad correspondiente (B, KB, MB, GB, TB).
 *
 * Ejemplos:
 *   formatearBytesHumanos(800)          -> "800.0 B"
 *   formatearBytesHumanos(1536)         -> "1.5 KB"
 *   formatearBytesHumanos(3221225472.0) -> "3.0 GB"
 *
 * @param bytes Valor en bytes (o bytes/s para tasas de red). Se acepta
 *              cualquier valor no negativo; valores negativos se tratan
 *              como su magnitud absoluta, preservando el signo en el
 *              resultado.
 * @return Cadena con el valor redondeado a 1 decimal seguido de la
 *         unidad elegida, separados por un espacio (p. ej. "1.5 KB").
 */
std::string formatearBytesHumanos(double bytes);

} // namespace chuspapulse::utils
