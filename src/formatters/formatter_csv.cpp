#include "formatter_csv.hpp"

#include <iomanip>
#include <sstream>

namespace chuspapulse::formatters {

static const std::string CABECERA =
    "timestamp,cpu_pct,ram_used_mb,ram_total_mb,"
    "disk_used_gb,disk_total_gb,net_rx_kb,net_tx_kb";

std::string FormatterCSV::formato() const {
    return "csv";
}

std::string FormatterCSV::contentType() const {
    return "text/csv";
}

std::string FormatterCSV::cabecera() {
    return CABECERA + "\n";
}

std::string FormatterCSV::filaDeSnapshot(const chuspapulse::core::Snapshot& snapshot) {
    double cpu_pct      = 0.0;
    double ram_used_mb  = 0.0;
    double ram_total_mb = 0.0;
    double disk_used_gb = 0.0;
    double disk_total_gb= 0.0;
    double net_rx_kb    = 0.0;
    double net_tx_kb    = 0.0;

    for (const auto& m : snapshot.metricas) {
        if (m.nombre == "cpu.usage")        cpu_pct      = m.valor;
        else if (m.nombre == "ram.used")    ram_used_mb  = m.valor / (1024.0 * 1024.0);
        else if (m.nombre == "ram.total")   ram_total_mb = m.valor / (1024.0 * 1024.0);
        else if (m.nombre == "disk.used")   disk_used_gb = m.valor / (1024.0 * 1024.0 * 1024.0);
        else if (m.nombre == "disk.total")  disk_total_gb= m.valor / (1024.0 * 1024.0 * 1024.0);
        else if (m.nombre == "net.rx" || m.nombre == "network.rx")      net_rx_kb    = m.valor / 1024.0;
        else if (m.nombre == "net.tx" || m.nombre == "network.tx")      net_tx_kb    = m.valor / 1024.0;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << snapshot.timestamp << ","
        << cpu_pct      << ","
        << ram_used_mb  << ","
        << ram_total_mb << ","
        << disk_used_gb << ","
        << disk_total_gb<< ","
        << net_rx_kb    << ","
        << net_tx_kb    << "\n";

    return oss.str();
}

std::string FormatterCSV::formatear(const chuspapulse::core::Snapshot& snapshot) const {
    std::string resultado;

    if (!cabecераEmitida_) {
        resultado += cabecera();
        cabecераEmitida_ = true;
    }

    resultado += filaDeSnapshot(snapshot);
    return resultado;
}

std::string FormatterCSV::formatearHistorial(
    const std::vector<chuspapulse::core::Snapshot>& snapshots) const {

    std::string resultado = cabecera();

    for (const auto& snapshot : snapshots) {
        resultado += filaDeSnapshot(snapshot);
    }

    return resultado;
}

} // namespace chuspapulse::formatters
