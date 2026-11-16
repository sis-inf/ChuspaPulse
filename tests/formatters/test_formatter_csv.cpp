#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
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

TEST(TestFormatterCSV, Test_MetricasDesconocidasSeIgnoran) {
    chuspapulse::formatters::FormatterCSV formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 3000;
    snapshot.metricas = {
        crearMetrica("gpu.usage", 99.0, "porcentaje", 3000),
        crearMetrica("", 50.0, "", 3000),
        crearMetrica("CPU.USAGE", 75.0, "porcentaje", 3000),
    };

    EXPECT_EQ(
        formatter.formatear(snapshot),
        CABECERA + "3000,0.00,0.00,0.00,0.00,0.00,0.00,0.00\n");
}

TEST(TestFormatterCSV, Test_ValoresNegativosNoSeValidan) {
    chuspapulse::formatters::FormatterCSV formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 4000;
    snapshot.metricas = {
        crearMetrica("cpu.usage", -5.0, "porcentaje", 4000),
        crearMetrica("ram.used", -1048576.0, "bytes", 4000),
    };

    EXPECT_EQ(
        formatter.formatear(snapshot),
        CABECERA + "4000,-5.00,-1.00,0.00,0.00,0.00,0.00,0.00\n");
}

TEST(TestFormatterCSV, Test_CpuSuperiorACienSeEmiteSinRecortar) {
    chuspapulse::formatters::FormatterCSV formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 5000;
    snapshot.metricas = {
        crearMetrica("cpu.usage", 150.0, "porcentaje", 5000),
        crearMetrica("disk.used", 1073741824.0, "bytes", 5000),
    };

    EXPECT_EQ(
        formatter.formatear(snapshot),
        CABECERA + "5000,150.00,0.00,0.00,1.00,0.00,0.00,0.00\n");
}

TEST(TestFormatterCSV, Test_MetricaDuplicadaPrevaleceLaUltima) {
    chuspapulse::formatters::FormatterCSV formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 6000;
    snapshot.metricas = {
        crearMetrica("cpu.usage", 10.0, "porcentaje", 6000),
        crearMetrica("cpu.usage", 20.0, "porcentaje", 6000),
    };

    EXPECT_EQ(
        formatter.formatear(snapshot),
        CABECERA + "6000,20.00,0.00,0.00,0.00,0.00,0.00,0.00\n");
}

TEST(TestFormatterCSV, Test_RedondeoADosDecimales) {
    chuspapulse::formatters::FormatterCSV formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 7000;
    snapshot.metricas = {
        crearMetrica("cpu.usage", 12.346, "porcentaje", 7000),
        crearMetrica("net.rx", 1.0, "bytes", 7000),
        crearMetrica("net.tx", 6.0, "bytes", 7000),
    };

    EXPECT_EQ(
        formatter.formatear(snapshot),
        CABECERA + "7000,12.35,0.00,0.00,0.00,0.00,0.00,0.01\n");
}

TEST(TestFormatterCSV, Test_ValoresNoFinitosGeneranFilaSinFallar) {
    chuspapulse::formatters::FormatterCSV formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 8000;
    snapshot.metricas = {
        crearMetrica(
            "cpu.usage", std::numeric_limits<double>::quiet_NaN(),
            "porcentaje", 8000),
        crearMetrica(
            "net.tx", std::numeric_limits<double>::infinity(),
            "bytes", 8000),
    };

    const std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("nan"), std::string::npos);
    EXPECT_NE(resultado.find("inf"), std::string::npos);
    EXPECT_EQ(resultado.back(), '\n');
}

TEST(TestFormatterCSV, Test_TimestampNegativoYMaximo) {
    chuspapulse::formatters::FormatterCSV formatter;

    chuspapulse::core::Snapshot negativo;
    negativo.timestamp = -1;

    chuspapulse::core::Snapshot maximo;
    maximo.timestamp = std::numeric_limits<std::int64_t>::max();

    EXPECT_EQ(
        formatter.formatearHistorial({negativo, maximo}),
        CABECERA +
            "-1,0.00,0.00,0.00,0.00,0.00,0.00,0.00\n" +
            "9223372036854775807,0.00,0.00,0.00,0.00,0.00,0.00,0.00\n");
}

TEST(TestFormatterCSV, Test_HistorialNoAlteraElEstadoDeLaCabecera) {
    chuspapulse::formatters::FormatterCSV formatter;

    ASSERT_EQ(
        formatter.formatearHistorial({crearSnapshotCompleto(1700000000)}),
        CABECERA + FILA_COMPLETA);

    EXPECT_EQ(
        formatter.formatear(crearSnapshotCompleto(1700000000)),
        CABECERA + FILA_COMPLETA);
}
