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

    // Preparar valores formateados como strings para aplicarles ancho fijo
    std::ostringstream cpu_oss;
    cpu_oss << std::fixed << std::setprecision(1) << cpu_pct << "%";

    std::string ram_str = chuspapulse::utils::formatearBytesHumanos(ram_usado_bytes) + "/" +
                          chuspapulse::utils::formatearBytesHumanos(ram_total_bytes);
    std::string disco_str = chuspapulse::utils::formatearBytesHumanos(disco_usado_bytes) + "/" +
                            chuspapulse::utils::formatearBytesHumanos(disco_total_bytes);
    std::string red_str = chuspapulse::utils::formatearBytesHumanos(net_rx_bytes) + "/s/" +
                          chuspapulse::utils::formatearBytesHumanos(net_tx_bytes) + "/s";

    // Formatear línea tabulada con anchos fijos usando std::setw y std::left
    std::ostringstream oss;
    oss << std::left
        << std::setw(20) << formatearTimestamp(snapshot.timestamp)
        << " | " << "CPU: " << std::setw(6) << cpu_oss.str()
        << " | " << "RAM: " << std::setw(18) << ram_str
        << " | " << "Disco: " << std::setw(22) << disco_str
        << " | " << "Red: " << red_str;

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
