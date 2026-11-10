#include "formatter_text.hpp"

#include <sstream>
#include <iomanip>
#include <ctime>

#include "utils/formato_bytes.hpp"

namespace chuspapulse::formatters {

std::string FormatterText::formato() const {
    return "text";
}

std::string FormatterText::contentType() const {
    return "text/plain";
}

// Convierte timestamp Unix (segundos) a string ISO 8601
static std::string formatearTimestamp(std::int64_t ts) {
    std::time_t tiempo = static_cast<std::time_t>(ts);
    std::tm* tm = std::gmtime(&tiempo);

    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string FormatterText::formatear(
    const chuspapulse::core::Snapshot& snapshot
) const {
    // Extraer métricas del snapshot (valores crudos en bytes / bytes-por-segundo)
    double cpu_pct = 0.0;
    double ram_usado_bytes = 0.0;
    double ram_total_bytes = 0.0;
    double disco_usado_bytes = 0.0;
    double disco_total_bytes = 0.0;
    double net_rx_bytes = 0.0;
    double net_tx_bytes = 0.0;

    for (const auto& metrica : snapshot.metricas) {
        if (metrica.nombre == "cpu.usage" || metrica.nombre == "cpu") {
            cpu_pct = metrica.valor;
        } else if (metrica.nombre == "ram.used" || metrica.nombre == "ram.usado") {
            ram_usado_bytes = metrica.valor;
        } else if (metrica.nombre == "ram.total" || metrica.nombre == "ram.total") {
            ram_total_bytes = metrica.valor;
        } else if (metrica.nombre == "disk.used" || metrica.nombre == "disco.usado") {
            disco_usado_bytes = metrica.valor;
        } else if (metrica.nombre == "disk.total" || metrica.nombre == "disco.total") {
            disco_total_bytes = metrica.valor;
        } else if (metrica.nombre == "network.rx" || metrica.nombre == "net.rx") {
            net_rx_bytes = metrica.valor;
        } else if (metrica.nombre == "network.tx" || metrica.nombre == "net.tx") {
            net_tx_bytes = metrica.valor;
        }
    }

    // Formatear línea: timestamp | CPU: 45.3% | RAM: 2.1 GB/8.0 GB | Disco: 120.0 GB/500.0 GB | Red: 1.2 KB/s/800.0 B/s
    std::ostringstream oss;
    oss << formatearTimestamp(snapshot.timestamp)
        << " | CPU: " << std::fixed << std::setprecision(1) << cpu_pct << "%"
        << " | RAM: " << chuspapulse::utils::formatearBytesHumanos(ram_usado_bytes)
        << "/" << chuspapulse::utils::formatearBytesHumanos(ram_total_bytes)
        << " | Disco: " << chuspapulse::utils::formatearBytesHumanos(disco_usado_bytes)
        << "/" << chuspapulse::utils::formatearBytesHumanos(disco_total_bytes)
        << " | Red: " << chuspapulse::utils::formatearBytesHumanos(net_rx_bytes) << "/s"
        << "/" << chuspapulse::utils::formatearBytesHumanos(net_tx_bytes) << "/s";

    return oss.str();
}

std::string FormatterText::formatearHistorial(
    const std::vector<chuspapulse::core::Snapshot>& snapshots
) const {
    std::ostringstream oss;

    for (const auto& snapshot : snapshots) {
        oss << formatear(snapshot) << "\n";
    }

    return oss.str();
}

} // namespace chuspapulse::formatters
