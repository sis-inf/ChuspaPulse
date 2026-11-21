#pragma once

#include <cctype>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "config/config.hpp"

namespace chuspapulse::http {

/**
 * @brief Códigos de estado HTTP relevantes para el controlador de configuración.
 */
enum class HttpStatusCode : int {
  Ok = 200,
  BadRequest = 400,
  InternalServerError = 500
};

/**
 * @brief Representa una respuesta HTTP estructurada en formato JSON.
 */
struct HttpResponse {
  int status_code{static_cast<int>(HttpStatusCode::Ok)};
  std::string content_type{"application/json; charset=utf-8"};
  std::string body;
};

/**
 * @brief Escapa caracteres especiales de una cadena para producir un JSON válido.
 *
 * @param input Cadena de texto sin escapar.
 * @return      Cadena segura para insertarse entre comillas en JSON.
 */
inline std::string escapeJsonString(std::string_view input) {
  std::string escaped;
  escaped.reserve(input.size());

  for (const unsigned char ch : input) {
    switch (ch) {
      case '"':
        escaped += "\\\"";
        break;
      case '\\':
        escaped += "\\\\";
        break;
      case '\b':
        escaped += "\\b";
        break;
      case '\f':
        escaped += "\\f";
        break;
      case '\n':
        escaped += "\\n";
        break;
      case '\r':
        escaped += "\\r";
        break;
      case '\t':
        escaped += "\\t";
        break;
      default:
        if (ch < 0x20) {
          static constexpr char kHexDigits[] = "0123456789abcdef";
          escaped += "\\u00";
          escaped += kHexDigits[(ch >> 4) & 0x0f];
          escaped += kHexDigits[ch & 0x0f];
        } else {
          escaped.push_back(static_cast<char>(ch));
        }
        break;
    }
  }

  return escaped;
}

/**
 * @brief Construye un objeto JSON de error estandarizado.
 *
 * @param status_code Código de estado HTTP asociado al error (por ejemplo, 400 o 500).
 * @param message     Descripción legible del fallo ocurrido.
 * @return            String JSON con `error` y `status`.
 */
inline std::string buildErrorJson(int status_code, std::string_view message) {
  return std::string("{\"status\":") + std::to_string(status_code) +
         ",\"error\":\"" + escapeJsonString(message) + "\"}";
}

/**
 * @brief Decodifica secuencias porcentuales (`%XX`) y `+` en un componente de query string.
 *
 * @param encoded Fragmento codificado de la query string.
 * @return        Cadena decodificada.
 * @throws        std::invalid_argument Si encuentra una secuencia `%` mal formada.
 */
inline std::string decodeQueryComponent(std::string_view encoded) {
  auto hexValue = [](char c) -> int {
    if (c >= '0' && c <= '9') {
      return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
      return 10 + (c - 'a');
    }
    if (c >= 'A' && c <= 'F') {
      return 10 + (c - 'A');
    }
    return -1;
  };

  std::string decoded;
  decoded.reserve(encoded.size());

  for (std::size_t i = 0; i < encoded.size(); ++i) {
    const char ch = encoded[i];
    if (ch == '+') {
      decoded.push_back(' ');
    } else if (ch == '%') {
      if (i + 2 >= encoded.size()) {
        throw std::invalid_argument("Secuencia porcentual incompleta en query string");
      }
      const int high = hexValue(encoded[i + 1]);
      const int low = hexValue(encoded[i + 2]);
      if (high < 0 || low < 0) {
        throw std::invalid_argument("Dígitos hexadecimales inválidos en query string");
      }
      decoded.push_back(static_cast<char>((high << 4) | low));
      i += 2;
    } else {
      decoded.push_back(ch);
    }
  }

  return decoded;
}

/**
 * @brief Parsea una query string HTTP (`clave=valor&...`) en un mapa de parámetros.
 *
 * @param query_string Cadena de consulta sin el `?` inicial (o con `?` inicial opcional).
 * @param params       Mapa de salida donde se almacenan los pares clave-valor decodificados.
 * @param error_msg    Mensaje de error si la query string contiene sintaxis inválida.
 * @return             `true` si el parsing fue válido, `false` si hubo error de formato (400).
 */
inline bool parseQueryString(
    std::string_view query_string,
    std::unordered_map<std::string, std::string>& params,
    std::string& error_msg) {
  params.clear();
  error_msg.clear();

  if (!query_string.empty() && query_string.front() == '?') {
    query_string.remove_prefix(1);
  }

  if (query_string.empty()) {
    return true;
  }

  std::size_t start = 0;
  while (start <= query_string.size()) {
    const std::size_t amp_pos = query_string.find('&', start);
    const std::size_t end =
        (amp_pos == std::string_view::npos) ? query_string.size() : amp_pos;
    const std::string_view token = query_string.substr(start, end - start);

    if (token.empty()) {
      error_msg = "Parámetro vacío en query string";
      return false;
    }

    const std::size_t eq_pos = token.find('=');
    const std::string_view raw_key =
        (eq_pos == std::string_view::npos) ? token : token.substr(0, eq_pos);
    const std::string_view raw_val =
        (eq_pos == std::string_view::npos) ? std::string_view{} : token.substr(eq_pos + 1);

    if (raw_key.empty()) {
      error_msg = "Nombre de parámetro vacío en query string";
      return false;
    }

    try {
      std::string key = decodeQueryComponent(raw_key);
      std::string val = decodeQueryComponent(raw_val);
      params[std::move(key)] = std::move(val);
    } catch (const std::invalid_argument& ex) {
      error_msg = ex.what();
      return false;
    }

    if (amp_pos == std::string_view::npos) {
      break;
    }
    start = amp_pos + 1;
  }

  return true;
}

/**
 * @brief Valida los parámetros de consulta admitidos por el endpoint de configuración.
 *
 * Actualmente rechaza parámetros desconocidos o valores vacíos explícitos.
 *
 * @param params    Mapa de parámetros ya parseados.
 * @param error_msg Mensaje descriptivo cuando un parámetro no es válido.
 * @return          `true` si todos los parámetros son válidos, `false` en caso contrario.
 */
inline bool validateConfigQueryParams(
    const std::unordered_map<std::string, std::string>& params,
    std::string& error_msg) {
  error_msg.clear();

  for (const auto& [key, value] : params) {
    if (value.empty()) {
      error_msg = "El parámetro '" + key + "' no puede estar vacío";
      return false;
    }
    error_msg = "Parámetro de consulta no soportado: '" + key + "'";
    return false;
  }

  return true;
}

/**
 * @brief Genera la respuesta JSON para el endpoint GET /config.
 *
 * Serializa los campos no sensibles de @p config en un objeto JSON plano.
 * El campo `interval_ms` se obtiene convirtiendo
 * `config.sampler.intervalo_segundos` a milisegundos (`× 1000`).
 *
 * Ejemplo de respuesta:
 * @code
 * {
 *   "interval_ms": 10000,
 *   "http_port": 8080,
 *   "log_level": "info",
 *   "output_format": "json",
 *   "version": "0.1.0"
 * }
 * @endcode
 *
 * @param config  Configuración activa cargada por `chuspapulse::config::cargar()`.
 * @return        String con el JSON listo para enviar como cuerpo HTTP.
 *
 * @note  La función es pura (sin efectos secundarios) y thread-safe: puede
 *        invocarse desde el callback de httplib sin sincronización adicional.
 *
 * @see chuspapulse::config::Config
 * @see chuspapulse::config::cargar()
 */
std::string handleConfig(const chuspapulse::config::Config& config);

/**
 * @brief Procesa una petición GET /config con query string y devuelve una respuesta HTTP controlada.
 *
 * Devuelve `400` ante query strings o parámetros inválidos, `500` si ocurre una
 * excepción controlada durante la generación de configuración, o `200` con JSON válido.
 *
 * @param config       Configuración activa cargada.
 * @param query_string Query string de la petición HTTP (por defecto vacía).
 * @return             Estructura `HttpResponse` con código de estado, cabecera y cuerpo JSON.
 */
inline HttpResponse buildConfigHttpResponse(
    const chuspapulse::config::Config& config,
    std::string_view query_string = {}) {
  HttpResponse response;

  try {
    std::unordered_map<std::string, std::string> params;
    std::string error_msg;

    if (!parseQueryString(query_string, params, error_msg)) {
      response.status_code = static_cast<int>(HttpStatusCode::BadRequest);
      response.body = buildErrorJson(response.status_code, error_msg);
      return response;
    }

    if (!validateConfigQueryParams(params, error_msg)) {
      response.status_code = static_cast<int>(HttpStatusCode::BadRequest);
      response.body = buildErrorJson(response.status_code, error_msg);
      return response;
    }

    response.status_code = static_cast<int>(HttpStatusCode::Ok);
    response.body = handleConfig(config);
    return response;
  } catch (const std::exception& ex) {
    response.status_code = static_cast<int>(HttpStatusCode::InternalServerError);
    response.body = buildErrorJson(response.status_code, ex.what());
    return response;
  } catch (...) {
    response.status_code = static_cast<int>(HttpStatusCode::InternalServerError);
    response.body = buildErrorJson(
        response.status_code,
        "Error interno inesperado al procesar la configuración");
    return response;
  }
}

} // namespace chuspapulse::http