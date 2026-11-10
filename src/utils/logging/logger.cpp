#include "logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace chuspapulse::utils::logging {

namespace {

std::string obtenerTimestamp() {
    auto ahora = std::chrono::system_clock::now();
    std::time_t tiempo = std::chrono::system_clock::to_time_t(ahora);
    std::tm tiempoLocal{};
#ifdef _WIN32
    localtime_s(&tiempoLocal, &tiempo);
#else
    localtime_r(&tiempo, &tiempoLocal);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tiempoLocal, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string nivelATexto(LogLevel nivel) {
    switch (nivel) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default:              return "UNKNOWN";
    }
}

void imprimir(const std::mutex& mutex_, LogLevel nivel, LogLevel minimo, const std::string& mensaje) {
    if (nivel < minimo) return;
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mutex_));
    std::cerr << "[" << obtenerTimestamp() << "] "
              << "[" << nivelATexto(nivel) << "] "
              << mensaje << '\n';
}

} // namespace

Logger& Logger::instancia() {
    static Logger instancia;
    return instancia;
}

void Logger::setMinLevel(LogLevel nivel) {
    nivelMinimo_ = nivel;
}

void Logger::debug(const std::string& mensaje) {
    imprimir(mutex_, LogLevel::DEBUG, nivelMinimo_, mensaje);
}

void Logger::info(const std::string& mensaje) {
    imprimir(mutex_, LogLevel::INFO, nivelMinimo_, mensaje);
}

void Logger::warn(const std::string& mensaje) {
    imprimir(mutex_, LogLevel::WARN, nivelMinimo_, mensaje);
}

void Logger::error(const std::string& mensaje) {
    imprimir(mutex_, LogLevel::ERROR, nivelMinimo_, mensaje);
}

} // namespace chuspapulse::utils::logging