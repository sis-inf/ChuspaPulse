#include "alerta_umbral.hpp"
#include <stdexcept>
#include <string>

namespace chuspapulse::alertas {

namespace {

/// Mapea el nombre de la métrica a su campo en MetricSnapshot.
/// Devuelve nullptr si la métrica no existe.
const double* campoDeMetrica(const std::string& metrica,
                             const chuspapulse::MetricSnapshot& snap) {
    if (metrica == "cpu" || metrica == "cpu.usage_pct") return &snap.cpu;
    if (metrica == "ram") return &snap.ram;
    if (metrica == "disk") return &snap.disk;
    if (metrica == "rx_bytes") return &snap.rx_bytes;
    if (metrica == "tx_bytes") return &snap.tx_bytes;
    return nullptr;
}

std::invalid_argument errorMetricaDesconocida(const std::string& metrica) {
    return std::invalid_argument(
        "Métrica desconocida: '" + metrica + "'. "
        "Métricas válidas: cpu, cpu.usage_pct, ram, disk, rx_bytes, tx_bytes."
    );
}

} // namespace

AlertaUmbral::AlertaUmbral(const std::string& metrica, float umbral, char operador)
    : metrica_(metrica), umbral_(umbral), operador_(operador)
{
    if (campoDeMetrica(metrica_, chuspapulse::MetricSnapshot{}) == nullptr) {
        throw errorMetricaDesconocida(metrica_);
    }

    if (operador_ != '>' && operador_ != '<') {
        throw std::invalid_argument(
            "Operador no válido: '" + std::string(1, operador_) +
            "'. Use '>' o '<'."
        );
    }
}

double AlertaUmbral::valorActual(const chuspapulse::MetricSnapshot& snap) const {
    const double* campo = campoDeMetrica(metrica_, snap);
    if (campo == nullptr) {
        throw errorMetricaDesconocida(metrica_);
    }
    return *campo;
}

bool AlertaUmbral::evaluar(const chuspapulse::MetricSnapshot& snap) const {
    const double valor = valorActual(snap);

    if (operador_ == '>') {
        return valor > static_cast<double>(umbral_);
    } else {
        return valor < static_cast<double>(umbral_);
    }
}

std::string AlertaUmbral::mensaje() const {
    return "Alerta: " + metrica_ + " " + operador_ +
           " " + std::to_string(umbral_);
}

} // namespace chuspapulse::alertas
