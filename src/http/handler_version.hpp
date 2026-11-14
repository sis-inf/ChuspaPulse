#ifndef HTTP_HANDLER_VERSION_HPP
#define HTTP_HANDLER_VERSION_HPP

#include <httplib.h>
#include <string>

namespace chuspapulse {
namespace http {

    /**
     * @brief Maneja la petición GET /version retornando un JSON con la versión de la app.
     *
     * Contrato:
     *  - No recibe parámetros: el endpoint no usa query strings ni cuerpo de la petición.
     *  - Retorna un JSON bien formado con la forma `{"version": "x.y.z"}`.
     *  - El código de estado HTTP lo fija quien registra la ruta (200 en el caso normal).
     *
     * @return JSON con la versión de la aplicación.
     */
    std::string handleVersion();

} // namespace http
} // namespace chuspapulse

#endif 