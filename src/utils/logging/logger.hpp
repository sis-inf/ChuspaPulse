#pragma once

#include <mutex>
#include <string>

namespace pulso::utils::logging {

/**
 * @brief Niveles de log disponibles para el Logger.
 * DEBUG=0, INFO=1, WARN=2, ERROR=3
 */
#ifdef ERROR
#undef ERROR
#endif

enum class LogLevel {
    DEBUG = 0,
    INFO  = 1,
    WARN  = 2,
    ERROR = 3
};

/**
 * @brief Logger singleton con soporte para niveles configurables.
 * Por defecto el nivel mínimo es INFO.
 */
class Logger {
public:
    static Logger& instancia();

    /**
     * @brief Establece el nivel mínimo de log.
     * Mensajes por debajo de este nivel no se imprimen.
     * @param nivel Nivel mínimo deseado.
     */
    void setMinLevel(LogLevel nivel);

    void debug(const std::string& mensaje);
    void info(const std::string& mensaje);
    void warn(const std::string& mensaje);
    void error(const std::string& mensaje);

private:
    Logger() = default;
    LogLevel nivelMinimo_ = LogLevel::INFO;
    std::mutex mutex_;
};

} // namespace pulso::utils::logging