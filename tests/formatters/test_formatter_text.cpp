#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "core/types.hpp"
#include "formatters/formatter_text.hpp"

namespace {

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
//   cpu 45.5 %, RAM 2 GB de 8 GB, disco 100 GB de 500 GB,
//   red rx 2 KB/s y tx 1 KB/s.
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

// 1700000000 = 2023-11-14T22:13:20Z
const std::string LINEA_COMPLETA =
    "2023-11-14T22:13:20Z | CPU: 45.5% | RAM: 2.0 GB/8.0 GB"
    " | Disco: 100.0 GB/500.0 GB | Red: 2.0 KB/s/1.0 KB/s";

} // namespace

TEST(TestFormatterText, Test_FormatoYContentType) {
    chuspapulse::formatters::FormatterText formatter;

    EXPECT_EQ(formatter.formato(), "text");
    EXPECT_EQ(formatter.contentType(), "text/plain");
}

TEST(TestFormatterText, Test_SnapshotConMetricasConocidas) {
    chuspapulse::formatters::FormatterText formatter;

    EXPECT_EQ(
        formatter.formatear(crearSnapshotCompleto(1700000000)),
        LINEA_COMPLETA);
}

TEST(TestFormatterText, Test_SnapshotVacioUsaCeros) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 0;

    EXPECT_EQ(
        formatter.formatear(snapshot),
        "1970-01-01T00:00:00Z | CPU: 0.0% | RAM: 0.0 B/0.0 B"
        " | Disco: 0.0 B/0.0 B | Red: 0.0 B/s/0.0 B/s");
}

TEST(TestFormatterText, Test_AliasNetRxNetTx) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 0;
    snapshot.metricas = {
        crearMetrica("net.rx", 800.0, "bytes", 0),
        crearMetrica("net.tx", 1536.0, "bytes", 0),
    };

    EXPECT_EQ(
        formatter.formatear(snapshot),
        "1970-01-01T00:00:00Z | CPU: 0.0% | RAM: 0.0 B/0.0 B"
        " | Disco: 0.0 B/0.0 B | Red: 800.0 B/s/1.5 KB/s");
}

TEST(TestFormatterText, Test_HistorialVacioDevuelveStringVacio) {
    chuspapulse::formatters::FormatterText formatter;

    EXPECT_EQ(formatter.formatearHistorial({}), "");
}

TEST(TestFormatterText, Test_HistorialUnaLineaPorSnapshot) {
    chuspapulse::formatters::FormatterText formatter;

    std::vector<chuspapulse::core::Snapshot> historial = {
        crearSnapshotCompleto(1700000000),
        crearSnapshotCompleto(1700000000),
    };

    EXPECT_EQ(
        formatter.formatearHistorial(historial),
        LINEA_COMPLETA + "\n" + LINEA_COMPLETA + "\n");
}
