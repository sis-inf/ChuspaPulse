#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "core/types.hpp"
#include "formatters/formatter_text.hpp"
#include "utils/formato_bytes.hpp"

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

bool contiene(const std::string& texto, const std::string& fragmento) {
    return texto.find(fragmento) != std::string::npos;
}

std::string usoYTotal(double usado, double total) {
    return chuspapulse::utils::formatearBytesHumanos(usado) + "/" +
           chuspapulse::utils::formatearBytesHumanos(total);
}

std::string trafico(double rx, double tx) {
    return chuspapulse::utils::formatearBytesHumanos(rx) + "/s/" +
           chuspapulse::utils::formatearBytesHumanos(tx) + "/s";
}

} // namespace

TEST(TestFormatterText, Test_FormatoYContentType) {
    chuspapulse::formatters::FormatterText formatter;

    EXPECT_EQ(formatter.formato(), "text");
    EXPECT_EQ(formatter.contentType(), "text/plain");
}

TEST(TestFormatterText, Test_SnapshotConMetricasConocidas) {
    chuspapulse::formatters::FormatterText formatter;

    const std::string salida =
        formatter.formatear(crearSnapshotCompleto(1700000000));

    EXPECT_TRUE(contiene(salida, "2023-11-14T22:13:20Z\n"));
    EXPECT_TRUE(contiene(salida, "  CPU       : 45.5    %"));
    EXPECT_TRUE(contiene(salida,
        "  RAM       : " + usoYTotal(2147483648.0, 8589934592.0)));
    EXPECT_TRUE(contiene(salida,
        "  Disco     : " + usoYTotal(107374182400.0, 536870912000.0)));
    EXPECT_TRUE(contiene(salida,
        "  Red       : " + trafico(2048.0, 1024.0)));
}

TEST(TestFormatterText, Test_SnapshotVacioUsaCeros) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 0;

    const std::string salida = formatter.formatear(snapshot);

    EXPECT_TRUE(contiene(salida, "1970-01-01T00:00:00Z\n"));
    EXPECT_TRUE(contiene(salida, "  CPU       : 0.0     %"));
    EXPECT_TRUE(contiene(salida, "  RAM       : " + usoYTotal(0.0, 0.0)));
    EXPECT_TRUE(contiene(salida, "  Disco     : " + usoYTotal(0.0, 0.0)));
    EXPECT_TRUE(contiene(salida, "  Red       : " + trafico(0.0, 0.0)));
}

TEST(TestFormatterText, Test_AliasNetRxNetTx) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 0;
    snapshot.metricas = {
        crearMetrica("net.rx", 800.0, "bytes", 0),
        crearMetrica("net.tx", 1536.0, "bytes", 0),
    };

    EXPECT_TRUE(contiene(formatter.formatear(snapshot),
        "  Red       : " + trafico(800.0, 1536.0)));
}

TEST(TestFormatterText, Test_HistorialVacioDevuelveStringVacio) {
    chuspapulse::formatters::FormatterText formatter;

    EXPECT_EQ(formatter.formatearHistorial({}), "");
}

TEST(TestFormatterText, Test_HistorialSeparaCadaSnapshot) {
    chuspapulse::formatters::FormatterText formatter;

    const auto snapshot = crearSnapshotCompleto(1700000000);
    const std::string bloque = formatter.formatear(snapshot);

    EXPECT_EQ(
        formatter.formatearHistorial({snapshot, snapshot}),
        bloque + "\n" + bloque + "\n");
}

TEST(TestFormatterText, Test_MetricaDesconocidaSeIgnora) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot vacio;
    vacio.timestamp = 0;
    chuspapulse::core::Snapshot snapshot = vacio;
    snapshot.metricas = {crearMetrica("gpu.usage", 99.0, "porcentaje", 0)};

    EXPECT_EQ(formatter.formatear(snapshot), formatter.formatear(vacio));
}

TEST(TestFormatterText, Test_NombreVacioSeIgnora) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot vacio;
    vacio.timestamp = 0;
    chuspapulse::core::Snapshot snapshot = vacio;
    snapshot.metricas = {crearMetrica("", 99.0, "porcentaje", 0)};

    EXPECT_EQ(formatter.formatear(snapshot), formatter.formatear(vacio));
}

TEST(TestFormatterText, Test_CpuNegativaSeMuestraSinCambios) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 0;
    snapshot.metricas = {crearMetrica("cpu.usage", -5.0, "porcentaje", 0)};

    EXPECT_TRUE(contiene(formatter.formatear(snapshot),
        "  CPU       : -5.0    %"));
}

TEST(TestFormatterText, Test_AliasCpu) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 0;
    snapshot.metricas = {crearMetrica("cpu", 80.0, "porcentaje", 0)};

    EXPECT_TRUE(contiene(formatter.formatear(snapshot),
        "  CPU       : 80.0    %"));
}

TEST(TestFormatterText, Test_MetricaRepetidaUsaLaUltima) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 0;
    snapshot.metricas = {
        crearMetrica("cpu.usage", 10.0, "porcentaje", 0),
        crearMetrica("cpu.usage", 20.0, "porcentaje", 0),
    };

    const std::string salida = formatter.formatear(snapshot);

    EXPECT_TRUE(contiene(salida, "  CPU       : 20.0    %"));
    EXPECT_FALSE(contiene(salida, "  CPU       : 10.0    %"));
}

TEST(TestFormatterText, Test_FaltaRamTotalUsaCero) {
    chuspapulse::formatters::FormatterText formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 0;
    snapshot.metricas = {
        crearMetrica("ram.used", 2147483648.0, "bytes", 0),
    };

    EXPECT_TRUE(contiene(formatter.formatear(snapshot),
        "  RAM       : " + usoYTotal(2147483648.0, 0.0)));
}