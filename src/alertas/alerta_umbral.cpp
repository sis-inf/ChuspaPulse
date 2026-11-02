#include "alerta_umbral.hpp"
#include <stdexcept>
#include <string>

namespace chuspapulse::alertas {

AlertaUmbral::AlertaUmbral(const std::string& metrica, float umbral, char operador)
    : metrica_(metrica), umbral_(umbral), operador_(operador)
{
    if (operador_ != '>' && operador_ != '<') {
        throw std::invalid_argument(
            "Operador no válido: '" + std::string(1, operador_) +
            "'. Use '>' o '<'."
        );
    }
}

bool AlertaUmbral::evaluar(const chuspapulse::MetricSnapshot& snap) const {
    double valor = 0.0;
    bool encontrado = true;

    // Mapear el nombre de la métrica a su campo en MetricSnapshot
    if (metrica_ == "cpu" || metrica_ == "cpu.usage_pct") {
        valor = snap.cpu;
    } else if (metrica_ == "ram") {
        valor = snap.ram;
    } else if (metrica_ == "disk") {
        valor = snap.disk;
    } else if (metrica_ == "rx_bytes") {
        valor = snap.rx_bytes;
    } else if (metrica_ == "tx_bytes") {
        valor = snap.tx_bytes;
    } else {
        encontrado = false;
    }

    if (!encontrado) {
        throw std::invalid_argument(
            "Métrica desconocida: '" + metrica_ + "'. "
            "Métricas válidas: cpu, cpu.usage_pct, ram, disk, rx_bytes, tx_bytes."
        );
    }

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
