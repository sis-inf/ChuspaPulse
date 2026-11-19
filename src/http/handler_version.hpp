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
     *  - Genera un JSON bien formado con el campo `version`, cuyo valor
     *    corresponde a `APP_VERSION`: `{"version": "x.y.z"}`.
     *  - No recibe la petición (ni `httplib::Request` ni `httplib::Response`):
     *    no lee parámetros, query strings ni cuerpo.
     *  - No decide el código de estado HTTP: eso pertenece a la ruta
     *    registrada en `src/main.cpp`.
     *
     * @return JSON con la versión de la aplicación (`APP_VERSION`).
     */
    std::string handleVersion();

} // namespace http
} // namespace chuspapulse

#endif 