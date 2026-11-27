#pragma once
#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>
#include <exception>
#include <iostream>
#include <utility>

namespace chuspapulse::collectors {

/**
 * @brief Excepción lanzada cuando un collector falla al recolectar métricas.
 */
class ErrorRecoleccion : public std::runtime_error {
public:
    /// @brief Constructor que acepta un mensaje de error.
    explicit ErrorRecoleccion(const std::string& mensaje)
        : std::runtime_error(normalizarMensaje(mensaje)), codigo_errno_(0) {
        registrarFallo(what());
        }

    /// @brief Constructor que acepta errno para describir el fallo del sistema.
    explicit ErrorRecoleccion(int codigo_errno)
        : std::runtime_error(formatearErrno(codigo_errno)), codigo_errno_(codigo_errno) {
        registrarFallo(what());
    }
    int codigoErrno() const noexcept {
        return codigo_errno_;
    }
template <typename Func, typename Fallback>
    static auto ejecutarConManejoControlado(
        const std::string& origen,
        Func&& operacion,
        Fallback&& valor_respaldo) noexcept -> std::decay_t<Fallback> {
        try {
            return std::forward<Func>(operacion)();
        } catch (const ErrorRecoleccion& ex) {
            registrarMensajeSeguro(origen, ex.what());
            return std::forward<Fallback>(valor_respaldo);
        } catch (const std::exception& ex) {
            registrarMensajeSeguro(origen, ex.what());
            return std::forward<Fallback>(valor_respaldo);
        } catch (...) {
            registrarMensajeSeguro(origen, "Error desconocido durante la recoleccion.");
            return std::forward<Fallback>(valor_respaldo);
        }
    }

private:
    int codigo_errno_ = 0;

    static std::string normalizarMensaje(const std::string& mensaje) {
        if (mensaje.empty()) {
            return "Fallo de recoleccion sin detalle especificado.";
        }
        return mensaje;
    }

    static std::string formatearErrno(int codigo_errno) {
        if (codigo_errno == 0) {
            return "Fallo de apertura o lectura del sistema sin codigo errno.";
        }
        const char* descripcion = std::strerror(codigo_errno);
        if (descripcion == nullptr || descripcion[0] == '\0') {
            return "Fallo del sistema en recoleccion (errno=" + std::to_string(codigo_errno) + ").";
        }
        return std::string(descripcion) + " (errno=" + std::to_string(codigo_errno) + ")";
    }

    static void registrarFallo(const char* detalle) noexcept {
        try {
            std::cerr << "[error_recoleccion] "
                      << (detalle != nullptr ? detalle : "Error de recoleccion.") << "\n";
        } catch (...) {
            // Evita propagar excepciones de E/S desde el registro de errores.
        }
    }

    static void registrarMensajeSeguro(const std::string& origen, const char* detalle) noexcept {
        try {
            std::cerr << "[error_recoleccion] Fallo controlado en '"
                      << (origen.empty() ? "colector" : origen) << "': "
                      << (detalle != nullptr ? detalle : "Error inesperado.") << "\n";
        } catch (...) {
            // Evita propagar excepciones de E/S desde el registro de errores.
        }
    }
};

} // namespace chuspapulse::collectors
