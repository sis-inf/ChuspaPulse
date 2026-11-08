#include "config.hpp"

// toml++ = header-only, incluir una sola vez en la TU que lo necesite.
#define TOML_EXCEPTIONS 1
#include <toml++/toml.hpp>

#include <filesystem>
#include <format>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

namespace chuspapulse::config {

namespace {

/// Lee un valor escalar del nodo TOML; si no existe devuelve `porDefecto`.
template <typename T>
T leer(const toml::table& tabla,
       std::string_view seccion,
       std::string_view clave,
       T porDefecto)
{
    if (auto* sec = tabla[seccion].as_table()) {
        if (auto nodo = (*sec)[clave]) {
            if (auto val = nodo.value<T>()) {
                return *val;
            }
        }
    }

    return porDefecto;
}

/// Sobrecarga sin sección (claves en la raíz del documento).
template <typename T>
T leer(const toml::table& tabla,
       std::string_view clave,
       T porDefecto)
{
    if (auto nodo = tabla[clave]) {
        if (auto val = nodo.value<T>()) {
            return *val;
        }
    }

    return porDefecto;
}


/// Aplica variables de entorno con prioridad sobre TOML.
void aplicarEnv(Config& cfg)
{
    if (const char* v = std::getenv("CHUSPAPULSE_SERVIDOR_HOST")) {
        cfg.servidor.host = v;
    }

    if (const char* v = std::getenv("CHUSPAPULSE_SERVIDOR_PUERTO")) {
        cfg.servidor.puerto = parsearPuerto(v);
    }

    if (const char* v = std::getenv("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS")) {
        cfg.sampler.intervalo_segundos = std::stoll(v);
    }

    if (const char* v = std::getenv("CHUSPAPULSE_STORAGE_RUTA_DB")) {
        cfg.storage.ruta_db = v;
    }

    if (const char* v = std::getenv("CHUSPAPULSE_NIVEL_LOG")) {
        cfg.nivel_log = v;
    }

    if (const char* v = std::getenv("CHUSPAPULSE_OUTPUT_FORMAT")) {
        cfg.output_format = v;
    }
}


/// Construye un Config a partir de un toml::table ya parseado.
Config mapear(const toml::table& doc)
{
    Config cfg;

    // [servidor]
    cfg.servidor.host =
        leer<std::string>(doc, "servidor", "host", cfg.servidor.host);

    // El puerto se valida sobre el valor original del TOML, antes de
    // convertirlo a int (así 4294975376 no se trunca a 8080).
    if (auto* sec = doc["servidor"].as_table()) {
        if (auto nodo = (*sec)["puerto"]) {
            auto valor = nodo.value_exact<int64_t>();
            if (!valor) {
                throw std::invalid_argument(
                    "Valor inválido para puerto: debe ser un entero entre 1 y 65535"
                );
            }
            if (*valor < 1 || *valor > 65535) {
                throw std::invalid_argument(
                    "Valor inválido para puerto: " + std::to_string(*valor) +
                    " (debe estar entre 1 y 65535)"
                );
            }
            cfg.servidor.puerto = static_cast<int>(*valor);
        }
    }


    // [sampler]
    cfg.sampler.intervalo_segundos =
        leer<int64_t>(doc, "sampler", "intervalo_segundos",
                      cfg.sampler.intervalo_segundos);


    // [storage]
    cfg.storage.ruta_db =
        leer<std::string>(doc, "storage", "ruta_db",
                          cfg.storage.ruta_db);

    // [umbrales]
    cfg.umbrales.cpu_max =
        leer<double>(doc, "umbrales", "cpu_max", cfg.umbrales.cpu_max);

    cfg.umbrales.ram_max =
        leer<double>(doc, "umbrales", "ram_max", cfg.umbrales.ram_max);

    cfg.umbrales.disco_max =
        leer<double>(doc, "umbrales", "disco_max", cfg.umbrales.disco_max);



    // nivel_log (clave raíz)
    cfg.nivel_log =
        leer<std::string>(doc, "nivel_log", cfg.nivel_log);


    // output_format (clave raíz)
    cfg.output_format =
        leer<std::string>(doc, "output_format",
                          cfg.output_format);


    aplicarEnv(cfg);

    return cfg;
}

} // namespace anónimo


Config cargar(const std::string& ruta)
{
    // Si el archivo no existe:
    // usar valores por defecto sin excepcion
    if (!std::filesystem::exists(ruta)) {
        Config cfg = porDefecto();
        aplicarEnv(cfg);
        return cfg;
    }


    try {
        toml::table doc = toml::parse_file(ruta);

        Config cfg = mapear(doc);


        // Validar nivel_log
        if (cfg.nivel_log != "debug" &&
            cfg.nivel_log != "info" &&
            cfg.nivel_log != "warn" &&
            cfg.nivel_log != "error") {

            throw std::invalid_argument(
                "Valor inválido para nivel_log: " + cfg.nivel_log
            );
        }


        // Validar output_format
        if (cfg.output_format != "json" &&
            cfg.output_format != "csv" &&
            cfg.output_format != "prometheus") {

            throw std::invalid_argument(
                "Valor inválido para output_format: " +
                cfg.output_format
            );
        }


        // Validar umbrales (porcentajes entre 0.0 y 100.0)
        if (!(cfg.umbrales.cpu_max >= 0.0 &&
              cfg.umbrales.cpu_max <= 100.0)) {

            throw std::invalid_argument(
                "Valor inválido para cpu_max: " +
                std::to_string(cfg.umbrales.cpu_max) +
                " (debe estar entre 0.0 y 100.0)"
            );
        }

        if (!(cfg.umbrales.ram_max >= 0.0 &&
              cfg.umbrales.ram_max <= 100.0)) {

            throw std::invalid_argument(
                "Valor inválido para ram_max: " +
                std::to_string(cfg.umbrales.ram_max) +
                " (debe estar entre 0.0 y 100.0)"
            );
        }

        if (!(cfg.umbrales.disco_max >= 0.0 &&
              cfg.umbrales.disco_max <= 100.0)) {

            throw std::invalid_argument(
                "Valor inválido para disco_max: " +
                std::to_string(cfg.umbrales.disco_max) +
                " (debe estar entre 0.0 y 100.0)"
            );
        }

        // Validar intervalo
        if (cfg.sampler.intervalo_segundos <= 0) {

            throw std::invalid_argument(
                "Valor inválido para intervalo_segundos: " +
                std::to_string(cfg.sampler.intervalo_segundos)
            );
        }


        return cfg;

    }
    catch (const toml::parse_error& e) {

        throw ErrorConfig(
            "Error parseando '" + ruta + "': " +
            std::string(e.description())
        );

    }
    catch (const std::invalid_argument&) {

        throw;

    }
    catch (const std::exception& e) {

        throw ErrorConfig(e.what());

    }
}


Config porDefecto()
{
    return Config{};
}


int parsearPuerto(const std::string& texto)
{
    if (texto.empty()) {
        throw std::invalid_argument("Valor inválido para puerto: vacío");
    }

    // Validar antes de convertir: solo dígitos y como mucho 5 (65535),
    // así std::stoi no acepta "8080abc", "-1" ni desborda.
    const bool soloDigitos =
        texto.find_first_not_of("0123456789") == std::string::npos;

    if (!soloDigitos || texto.size() > 5) {
        throw std::invalid_argument(
            "Valor inválido para puerto: '" + texto +
            "' (debe ser un entero entre 1 y 65535)"
        );
    }

    const int puerto = std::stoi(texto);

    if (puerto < 1 || puerto > 65535) {
        throw std::invalid_argument(
            "Valor inválido para puerto: " + texto +
            " (debe estar entre 1 y 65535)"
        );
    }

    return puerto;
}

} // namespace chuspapulse::config
