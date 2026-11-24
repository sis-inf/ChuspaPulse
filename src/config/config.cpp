#include "config.hpp"

// toml++ = header-only, incluir una sola vez en la TU que lo necesite.
#ifndef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 1
#endif
#include <toml++/toml.hpp>

#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>

namespace chuspapulse::config {

namespace {

template <typename T>
T leer(toml::node_view<const toml::node> nodo, T porDefecto)
{
    return nodo.value<T>().value_or(porDefecto);
}

toml::node_view<const toml::node> preferir(
    toml::node_view<const toml::node> nuevo,
    toml::node_view<const toml::node> anterior)
{
    return nuevo ? nuevo : anterior;
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


/// Valida que output_format sea uno de los formatos soportados.
void validarOutputFormat(const Config& cfg)
{
    if (cfg.output_format != "json" &&
        cfg.output_format != "csv" &&
        cfg.output_format != "prometheus" &&
        cfg.output_format != "text") {

        throw std::invalid_argument(
            "Valor inválido para output_format: " +
            cfg.output_format
        );
    }
}


/// Construye un Config a partir de un toml::table ya parseado.
Config mapear(const toml::table& doc)
{
    Config cfg;

    const auto http = doc["http"];
    const auto servidor = doc["servidor"];
    cfg.servidor.host =
        leer(preferir(http["host"], servidor["host"]), cfg.servidor.host);

    // El puerto se valida sobre el valor original del TOML, antes de
    // convertirlo a int (así 4294975376 no se trunca a 8080).
    if (auto nodo = preferir(http["puerto"], servidor["puerto"])) {
        auto valor = nodo.value_exact<std::int64_t>();
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


    // [sampler]
    cfg.sampler.intervalo_segundos =
        leer<std::int64_t>(doc["sampler"]["intervalo_segundos"],
                      cfg.sampler.intervalo_segundos);


    // [storage]
    cfg.storage.ruta_db =
        leer(doc["storage"]["ruta_db"], cfg.storage.ruta_db);

    const auto alertas = doc["alertas"];
    const auto umbrales = doc["umbrales"];
    cfg.umbrales.cpu_max =
        leer(preferir(alertas["cpu_max"], umbrales["cpu_max"]), cfg.umbrales.cpu_max);

    cfg.umbrales.ram_max =
        leer(preferir(alertas["ram_max"], umbrales["ram_max"]), cfg.umbrales.ram_max);

    cfg.umbrales.disco_max =
        leer(preferir(alertas["disco_max"], umbrales["disco_max"]), cfg.umbrales.disco_max);



    const auto general = doc["general"];
    cfg.nivel_log =
        leer(preferir(general["nivel_log"], doc["nivel_log"]), cfg.nivel_log);


    cfg.output_format =
        leer(preferir(general["output_format"], doc["output_format"]), cfg.output_format);


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
        validarOutputFormat(cfg);
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
        validarOutputFormat(cfg);


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


bool recargar(const std::string& ruta, Config& destino, std::string& error)
{
    if (!std::filesystem::exists(ruta)) {
        error = "El archivo de configuración no existe: " + ruta;
        return false;
    }

    try {
        Config nueva = cargar(ruta);
        destino = nueva;
        return true;
    }
    catch (const std::exception& e) {
        error = e.what();
        return false;
    }
}


std::vector<std::string> camposModificados(const Config& antes,
                                           const Config& despues)
{
    std::vector<std::string> campos;

    if (antes.servidor.host != despues.servidor.host)
        campos.push_back("servidor.host");
    if (antes.servidor.puerto != despues.servidor.puerto)
        campos.push_back("servidor.puerto");
    if (antes.sampler.intervalo_segundos != despues.sampler.intervalo_segundos)
        campos.push_back("sampler.intervalo_segundos");
    if (antes.storage.ruta_db != despues.storage.ruta_db)
        campos.push_back("storage.ruta_db");
    if (antes.umbrales.cpu_max != despues.umbrales.cpu_max)
        campos.push_back("umbrales.cpu_max");
    if (antes.umbrales.ram_max != despues.umbrales.ram_max)
        campos.push_back("umbrales.ram_max");
    if (antes.umbrales.disco_max != despues.umbrales.disco_max)
        campos.push_back("umbrales.disco_max");
    if (antes.nivel_log != despues.nivel_log)
        campos.push_back("nivel_log");
    if (antes.output_format != despues.output_format)
        campos.push_back("output_format");

    return campos;
}

} // namespace chuspapulse::config
