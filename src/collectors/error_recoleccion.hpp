#pragma once
#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>

namespace chuspapulse::collectors {

/**
 * @brief Excepción lanzada cuando un collector falla al recolectar métricas.
 */
class ErrorRecoleccion : public std::runtime_error {
public:
    /// @brief Constructor que acepta un mensaje de error.
    explicit ErrorRecoleccion(const std::string& mensaje)
        : std::runtime_error(mensaje) {}

    /// @brief Constructor que acepta errno para describir el fallo del sistema.
    explicit ErrorRecoleccion(int codigo_errno)
        : std::runtime_error(std::strerror(codigo_errno)) {}
};

} // namespace chuspapulse::collectors
