#pragma once

#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace chuspapulse::http {
struct RespuestaHttpHealth {
    int status = 200;
    std::string content_type = "application/json; charset=utf-8";
    std::string cuerpo;
};

inline std::string escaparJsonHealth(std::string_view texto) {
    std::string salida;
    salida.reserve(texto.size());
    for (unsigned char c : texto) {
        switch (c) {
            case '"':  salida += "\\\""; break;
            case '\\': salida += "\\\\"; break;
            case '\b': salida += "\\b"; break;
            case '\f': salida += "\\f"; break;
            case '\n': salida += "\\n"; break;
            case '\r': salida += "\\r"; break;
            case '\t': salida += "\\t"; break;
            default:
                if (c < 0x20) {
                    salida += ' ';
                } else {
                    salida += static_cast<char>(c);
                }
                break;
        }
    }
    return salida;
}

inline RespuestaHttpHealth construirErrorJsonHealth(
    int status,
    std::string_view codigo_error,
    std::string_view mensaje) {
    RespuestaHttpHealth resp{};
    resp.status = status;
    resp.content_type = "application/json; charset=utf-8";
    resp.cuerpo = std::string("{\"status\":\"error\",\"code\":") + std::to_string(status)
        + ",\"error\":\"" + escaparJsonHealth(codigo_error)
        + "\",\"message\":\"" + escaparJsonHealth(mensaje) + "\"}";
    return resp;
}

inline bool validarQueryHealth(std::string_view query_string, std::string& detalle_error) {
    if (query_string.empty()) {
        return true;
    }
    if (query_string.front() == '?') {
        query_string.remove_prefix(1);
    }
    if (query_string.size() > 512) {
        detalle_error = "El query string excede la longitud maxima permitida (512 caracteres).";
        return false;
    }
    for (unsigned char c : query_string) {
        if (c < 0x20 || c == 0x7F || c == ' ') {
            detalle_error = "El query string contiene caracteres invalidos o espacios sin codificar.";
            return false;
        }
    }

    std::size_t inicio = 0;
    while (inicio <= query_string.size()) {
        std::size_t fin = query_string.find('&', inicio);
        std::string_view par = (fin == std::string_view::npos)
            ? query_string.substr(inicio)
            : query_string.substr(inicio, fin - inicio);

        if (!par.empty()) {
            std::size_t eq = par.find('=');
            std::string_view clave = (eq == std::string_view::npos) ? par : par.substr(0, eq);
            if (clave.empty()) {
                detalle_error = "Parametro de consulta mal formado: nombre de parametro vacio.";
                return false;
            }
            if (clave != "detail" && clave != "format" && clave != "verbose") {
                detalle_error = "Parametro de consulta no soportado: '" + std::string(clave) + "'.";
                return false;
            }
        }

        if (fin == std::string_view::npos) {
            break;
        }
        inicio = fin + 1;
    }
    return true;
}

/**
 * @brief Genera una respuesta JSON para el endpoint GET /health.
 *
 * Retorna un JSON con:
 * - status: siempre "ok"
 * - uptime_seconds: segundos transcurridos desde start_time
 * - version: versión hardcodeada "0.1.0"
 * - hostname: nombre real de la máquina
 *
 * @param start_time Punto en el tiempo cuando el agente inició.
 * @return std::string JSON con la información de health-check.
 */
std::string handleHealth(std::chrono::steady_clock::time_point start_time);

/// Comprobación de un componente requerido: nombre y función que indica
/// si está disponible.
using ComprobacionReadiness = std::pair<std::string, std::function<bool()>>;

/// Resultado del endpoint GET /ready.
struct ResultadoReadiness {
    int status;          ///< 200 si todos los componentes están disponibles, 503 si no.
    std::string cuerpo;  ///< JSON con el estado general y el de cada componente.
};

/**
 * @brief Genera la respuesta del endpoint GET /ready.
 *
 * A diferencia de /health, que solo indica que el proceso está vivo,
 * readiness indica si el servicio puede atender solicitudes: evalúa cada
 * componente requerido y responde:
 * - 200 `{"status":"ready","componentes":{...}}` si todos están disponibles.
 * - 503 `{"status":"not_ready","componentes":{...}}` si alguno no lo está.
 *
 * Una comprobación que lanza una excepción cuenta como no disponible.
 *
 * @param comprobaciones Componentes requeridos y su comprobación.
 * @return ResultadoReadiness con el código HTTP y el cuerpo JSON.
 */
ResultadoReadiness handleReadiness(
    const std::vector<ComprobacionReadiness>& comprobaciones);
inline RespuestaHttpHealth handleHealthHttp(
    std::chrono::steady_clock::time_point start_time,
    std::string_view query_string = "") {
    std::string detalle_error;
    if (!validarQueryHealth(query_string, detalle_error)) {
        std::cerr << "[handler_health] Solicitud invalida en /health (400): " << detalle_error << "\n";
        return construirErrorJsonHealth(400, "bad_request", detalle_error);
    }

    try {
        std::string cuerpo = handleHealth(start_time);
        if (cuerpo.empty()) {
            std::cerr << "[handler_health] Cuerpo JSON vacio generado en /health (500).\n";
            return construirErrorJsonHealth(500, "internal_error", "Respuesta de health-check vacia.");
        }
        return RespuestaHttpHealth{200, "application/json; charset=utf-8", std::move(cuerpo)};
    } catch (const std::exception& ex) {
        std::cerr << "[handler_health] Error interno en /health (500): " << ex.what() << "\n";
        return construirErrorJsonHealth(500, "internal_error", ex.what());
    } catch (...) {
        std::cerr << "[handler_health] Error interno desconocido en /health (500).\n";
        return construirErrorJsonHealth(500, "internal_error", "Error interno al evaluar estado de salud.");
    }
}

inline RespuestaHttpHealth handleReadinessHttp(
    const std::vector<ComprobacionReadiness>& comprobaciones,
    std::string_view query_string = "") {
    std::string detalle_error;
    if (!validarQueryHealth(query_string, detalle_error)) {
        std::cerr << "[handler_health] Solicitud invalida en /ready (400): " << detalle_error << "\n";
        return construirErrorJsonHealth(400, "bad_request", detalle_error);
    }

    for (const auto& [nombre_comp, check] : comprobaciones) {
        if (nombre_comp.empty() || !check) {
            std::cerr << "[handler_health] Parametro de comprobacion invalido en /ready (400).\n";
            return construirErrorJsonHealth(
                400,
                "invalid_component",
                "Cada comprobacion de readiness debe tener un nombre no vacio y una funcion valida."
            );
        }
    }

    try {
        ResultadoReadiness res = handleReadiness(comprobaciones);
        int codigo = (res.status == 200 || res.status == 503) ? res.status : 500;
        if (res.cuerpo.empty()) {
            return construirErrorJsonHealth(500, "internal_error", "Respuesta de readiness vacia.");
        }
        return RespuestaHttpHealth{codigo, "application/json; charset=utf-8", std::move(res.cuerpo)};
    } catch (const std::exception& ex) {
        std::cerr << "[handler_health] Error interno en /ready (500): " << ex.what() << "\n";
        return construirErrorJsonHealth(500, "internal_error", ex.what());
    } catch (...) {
        std::cerr << "[handler_health] Error interno desconocido en /ready (500).\n";
        return construirErrorJsonHealth(500, "internal_error", "Error interno al evaluar readiness.");
    }
}

} // namespace chuspapulse::http
