#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "core/types.hpp"
#include "formatters/formatter_csv.hpp"

namespace {

const std::string CABECERA =
    "timestamp,cpu_pct,ram_used_mb,ram_total_mb,"
    "disk_used_gb,disk_total_gb,net_rx_kb,net_tx_kb\n";

chuspapulse::core::Metrica crearMetrica(
    const std::string& nombre,
    double valor,
    const std::string& unidad,
    std::int64_t timestamp) {
    chuspapulse::core::Metrica metrica;
    metrica.nombre = nombre;
    metrica.valor = valor;
    metrica.unidad = unidad;
    metrica.timestamp = timestamp;
    return metrica;
}

// Snapshot con valores conocidos:
//   cpu 45.5 %, RAM 2048 MB de 8192 MB, disco 100 GB de 500 GB,
//   red rx 2 KB y tx 1 KB.
chuspapulse::core::Snapshot crearSnapshotCompleto(std::int64_t timestamp) {
    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = timestamp;
    snapshot.metricas = {
        crearMetrica("cpu.usage", 45.5, "porcentaje", timestamp),
        crearMetrica("ram.used", 2147483648.0, "bytes", timestamp),
        crearMetrica("ram.total", 8589934592.0, "bytes", timestamp),
        crearMetrica("disk.used", 107374182400.0, "bytes", timestamp),
        crearMetrica("disk.total", 536870912000.0, "bytes", timestamp),
        crearMetrica("network.rx", 2048.0, "bytes", timestamp),
        crearMetrica("network.tx", 1024.0, "bytes", timestamp),
    };
    return snapshot;
}

const std::string FILA_COMPLETA =
    "1700000000,45.50,2048.00,8192.00,100.00,500.00,2.00,1.00\n";

} // namespace

TEST(TestFormatterCSV, Test_FormatoYContentType) {
    chuspapulse::formatters::FormatterCSV formatter;

    EXPECT_EQ(formatter.formato(), "csv");
    EXPECT_EQ(formatter.contentType(), "text/csv");
}

TEST(TestFormatterCSV, Test_PrimeraLlamadaIncluyeCabeceraYFila) {
    chuspapulse::formatters::FormatterCSV formatter;

    EXPECT_EQ(
        formatter.formatear(crearSnapshotCompleto(1700000000)),
        CABECERA + FILA_COMPLETA);
}

TEST(TestFormatterCSV, Test_SegundaLlamadaSoloEmiteFila) {
    chuspapulse::formatters::FormatterCSV formatter;

    formatter.formatear(crearSnapshotCompleto(1700000000));

    EXPECT_EQ(
        formatter.formatear(crearSnapshotCompleto(1700000000)),
        FILA_COMPLETA);
}

TEST(TestFormatterCSV, Test_SnapshotVacioUsaCeros) {
    chuspapulse::formatters::FormatterCSV formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 1000;

    EXPECT_EQ(
        formatter.formatear(snapshot),
        CABECERA + "1000,0.00,0.00,0.00,0.00,0.00,0.00,0.00\n");
}

TEST(TestFormatterCSV, Test_AliasNetRxNetTx) {
    chuspapulse::formatters::FormatterCSV formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 2000;
    snapshot.metricas = {
        crearMetrica("net.rx", 4096.0, "bytes", 2000),
        crearMetrica("net.tx", 3072.0, "bytes", 2000),
    };

    EXPECT_EQ(
        formatter.formatear(snapshot),
        CABECERA + "2000,0.00,0.00,0.00,0.00,0.00,4.00,3.00\n");
}

TEST(TestFormatterCSV, Test_HistorialVacioSoloCabecera) {
    chuspapulse::formatters::FormatterCSV formatter;

    EXPECT_EQ(formatter.formatearHistorial({}), CABECERA);
}

TEST(TestFormatterCSV, Test_HistorialConDosSnapshots) {
    chuspapulse::formatters::FormatterCSV formatter;

    std::vector<chuspapulse::core::Snapshot> historial = {
        crearSnapshotCompleto(1700000000),
        crearSnapshotCompleto(1700000010),
    };

    EXPECT_EQ(
        formatter.formatearHistorial(historial),
        CABECERA +
            "1700000000,45.50,2048.00,8192.00,100.00,500.00,2.00,1.00\n" +
            "1700000010,45.50,2048.00,8192.00,100.00,500.00,2.00,1.00\n");
}
