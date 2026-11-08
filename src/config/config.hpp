#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace chuspapulse::config {

// ---------------------------------------------------------------------------
// Excepción
// ---------------------------------------------------------------------------

/// Lanzada cuando el archivo de configuración no existe o contiene errores.
class ErrorConfig : public std::runtime_error {
public:
    explicit ErrorConfig(const std::string& mensaje)
        : std::runtime_error(mensaje) {}
};


struct UmbralesConfig {
    double cpu_max = 80.0;       // Umbral de porcentaje de CPU
    double ram_max = 85.0;       // Umbral de porcentaje de RAM
    double disco_max = 90.0;     // Umbral de porcentaje de Disco
};
// ---------------------------------------------------------------------------
// Estructuras de configuración
// ---------------------------------------------------------------------------
/// Configuración del monitor del sistema.
struct MonitorConfig
{
    /// Intervalo de actualización de métricas en milisegundos.
    int interval_ms = 1000;

    /// Habilita la lectura de métricas de CPU.
    bool cpu = true;

    /// Habilita la lectura de métricas de RAM.
    bool ram = true;

    /// Habilita la lectura de métricas de disco.
    bool disk = true;

    /// Constructor con valores por defecto.
    MonitorConfig() = default;
    
};
struct ConfigServidor {
    std::string host  = "0.0.0.0";
    int         puerto = 8080;
};

struct ConfigSampler {
    int intervalo_segundos = 10;
};

struct ConfigStorage {
    std::string ruta_db = "chuspapulse.db";
};

struct Config {
    ConfigServidor servidor;
    ConfigSampler  sampler;
    ConfigStorage  storage;
    UmbralesConfig umbrales;
    std::string    nivel_log = "info";
    /// Formato de salida de métricas.
    /// Valores válidos: json, csv, prometheus, text
    std::string    output_format = "json";
};

// ---------------------------------------------------------------------------
// API pública
// ---------------------------------------------------------------------------

/// Carga el archivo TOML ubicado en `ruta`.
/// Lanza ErrorConfig si la ruta no existe o el archivo es inválido.
Config cargar(const std::string& ruta);

/// Devuelve una Config construida con todos los valores por defecto.
Config porDefecto();

/// Vuelve a leer y validar el archivo TOML ubicado en `ruta`.
/// Si es válido, lo copia en `destino` y devuelve true.
/// Si falla, deja `destino` sin cambios, describe el motivo en `error`
/// y devuelve false. A diferencia de cargar(), un archivo inexistente
/// se considera un error.
bool recargar(const std::string& ruta, Config& destino, std::string& error);

/// Devuelve los nombres de los campos que difieren entre `antes` y `despues`
/// (por ejemplo "servidor.puerto" o "umbrales.cpu_max").
std::vector<std::string> camposModificados(const Config& antes,
                                           const Config& despues);

} // namespace chuspapulse::config
