#include "handler_health.hpp"
#include "core/version.hpp"

#include <chrono>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

namespace chuspapulse::http {

namespace {

std::string escaparJson(std::string_view texto) {
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

std::string construirRespuestaErrorJson(
    int status,
    std::string_view codigo_error,
    std::string_view mensaje) {
    std::ostringstream json;
    json << "{"
         << "\"status\":\"error\","
         << "\"code\":" << status << ","
         << "\"error\":\"" << escaparJson(codigo_error) << "\","
         << "\"message\":\"" << escaparJson(mensaje) << "\""
         << "}";
    return json.str();
}

bool validarQueryString(std::string_view query, std::string& detalle_error) {
    if (query.empty()) {
        return true;
    }
    if (query.front() == '?') {
        query.remove_prefix(1);
    }
    if (query.size() > 512) {
        detalle_error = "El query string excede el limite maximo de 512 caracteres.";
        return false;
    }
    for (unsigned char c : query) {
        if (c < 0x20 || c == 0x7F || c == ' ') {
            detalle_error = "El query string contiene caracteres de control o espacios sin codificar.";
            return false;
        }
    }

    std::size_t inicio = 0;
    while (inicio <= query.size()) {
        std::size_t fin = query.find('&', inicio);
        std::string_view par = (fin == std::string_view::npos)
            ? query.substr(inicio)
            : query.substr(inicio, fin - inicio);

        if (!par.empty()) {
            std::size_t eq = par.find('=');
            if (eq == std::string_view::npos) {
                detalle_error = "Parametro mal formado en query string: '" + std::string(par) + "'.";
                return false;
            }
            std::string_view clave = par.substr(0, eq);
            std::string_view valor = par.substr(eq + 1);
            if (clave.empty() || valor.empty()) {
                detalle_error = "Parametro o valor vacio en query string.";
                return false;
            }
            if (clave == "detail" || clave == "verbose") {
                if (valor != "true" && valor != "false" && valor != "1" && valor != "0") {
                    detalle_error = "Valor invalido para '" + std::string(clave) + "': '" + std::string(valor) + "'.";
                    return false;
                }
            } else if (clave == "format") {
                if (valor != "json") {
                    detalle_error = "Formato no soportado en 'format': '" + std::string(valor) + "'.";
                    return false;
                }
            } else {
                detalle_error = "Parametro de consulta no permitido: '" + std::string(clave) + "'.";
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

}

std::string handleHealth(std::chrono::steady_clock::time_point start_time) {
try {
        if (start_time == std::chrono::steady_clock::time_point{}) {
            std::cerr << "[handler_health] Parametro start_time no inicializado en handleHealth (400).\n";
            return construirRespuestaErrorJson(400, "bad_request", "Parametro start_time invalido.");
        }

        auto ahora = std::chrono::steady_clock::now();
        if (start_time > ahora) {
            std::cerr << "[handler_health] Parametro start_time futuro recibido en handleHealth (400).\n";
            return construirRespuestaErrorJson(400, "bad_request", "Parametro start_time no puede ser futuro.");
        }

        auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
                          ahora - start_time).count();

        // Obtener hostname de forma segura
        char hostname[256] = {0};
        if (gethostname(hostname, sizeof(hostname) - 1) != 0 || hostname[0] == '\0') {
            std::strncpy(hostname, "unknown", sizeof(hostname) - 1);
            hostname[sizeof(hostname) - 1] = '\0';
        }
        hostname[sizeof(hostname) - 1] = '\0';

        // Construir JSON bien formado con escape de cadenas
        std::ostringstream json;
        json << "{"
             << "\"status\":\"ok\","
             << "\"uptime_seconds\":" << uptime << ","
             << "\"version\":\"" << escaparJson(chuspapulse::APP_VERSION) << "\","
             << "\"hostname\":\"" << escaparJson(hostname) << "\""
             << "}";

        return json.str();
    } catch (const std::exception& ex) {
        std::cerr << "[handler_health] Error interno en handleHealth (500): " << ex.what() << "\n";
        return construirRespuestaErrorJson(500, "internal_error", ex.what());
    } catch (...) {
        std::cerr << "[handler_health] Error interno desconocido en handleHealth (500).\n";
        return construirRespuestaErrorJson(500, "internal_error", "Error interno al generar health-check.");
    }
}

ResultadoReadiness handleReadiness(
    const std::vector<ComprobacionReadiness>& comprobaciones) {
    try {
        // Validar parametros de entrada antes de ejecutar comprobaciones
        for (const auto& [nombre, comprobar] : comprobaciones) {
            if (nombre.empty() || !comprobar) {
                std::cerr << "[handler_health] Parametro de comprobacion invalido en handleReadiness (400).\n";
                return {
                    400,
                    construirRespuestaErrorJson(
                        400,
                        "bad_request",
                        "Cada comprobacion debe tener un nombre no vacio y una funcion valida."
                    )
                };
            }
        }

        bool listo = true;
        std::ostringstream componentes;
        componentes << "{";

        for (std::size_t i = 0; i < comprobaciones.size(); ++i) {
            const auto& [nombre, comprobar] = comprobaciones[i];

            bool disponible = false;
            try {
                disponible = comprobar();
            } catch (const std::exception& ex) {
                std::cerr << "[handler_health] Fallo en comprobacion '" << nombre << "': " << ex.what() << "\n";
                disponible = false;
            } catch (...) {
                std::cerr << "[handler_health] Fallo desconocido en comprobacion '" << nombre << "'.\n";
                disponible = false;
            }

            listo = listo && disponible;

            if (i > 0) {
                componentes << ",";
            }
            componentes << "\"" << escaparJson(nombre) << "\":"
                        << (disponible ? "true" : "false");
        }

        componentes << "}";

        std::ostringstream json;
        json << "{"
             << "\"status\":\"" << (listo ? "ready" : "not_ready") << "\","
             << "\"componentes\":" << componentes.str()
             << "}";

        return {listo ? 200 : 503, json.str()};
    } catch (const std::exception& ex) {
        std::cerr << "[handler_health] Error interno en handleReadiness (500): " << ex.what() << "\n";
        return {500, construirRespuestaErrorJson(500, "internal_error", ex.what())};
    } catch (...) {
        std::cerr << "[handler_health] Error interno desconocido en handleReadiness (500).\n";
        return {500, construirRespuestaErrorJson(500, "internal_error", "Error interno al evaluar readiness.")};
    }
}

ResultadoReadiness handleReadinessConQuery(
    const std::vector<ComprobacionReadiness>& comprobaciones,
    std::string_view query_string) {
    std::string detalle_error;
    if (!validarQueryString(query_string, detalle_error)) {
        std::cerr << "[handler_health] Query string invalido en readiness (400): " << detalle_error << "\n";
        return {400, construirRespuestaErrorJson(400, "bad_request", detalle_error)};
    }
    return handleReadiness(comprobaciones);
}

} // namespace chuspapulse::http
