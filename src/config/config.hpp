#pragma once

#include <stdexcept>
#include <string>

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
    /// Valores válidos: json, csv, prometheus
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

/// Valida el texto de un puerto y recién después lo convierte.
/// Acepta solo enteros entre 1 y 65535, escritos únicamente con dígitos
/// (sin signo, espacios, decimales ni sufijos).
/// Lanza std::invalid_argument con un mensaje explícito si no es válido.
int parsearPuerto(const std::string& texto);

} // namespace chuspapulse::config
