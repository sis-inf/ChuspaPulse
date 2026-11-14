
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "formatters/formatter_text.hpp"
#include "core/types.hpp"

using chuspapulse::core::Snapshot;
using chuspapulse::formatters::FormatterText;

TEST(FormatterTextTest, FormatoDevuelveText) {
    FormatterText formatter;

    EXPECT_EQ(formatter.formato(), "text");
}

TEST(FormatterTextTest, ContentTypeDevuelveTextPlain) {
    FormatterText formatter;

    EXPECT_EQ(formatter.contentType(), "text/plain");
}

TEST(FormatterTextTest, FormateaTimestampYValorDeCpu) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.timestamp = 0;
    snapshot.metricas = {
        {"cpu.usage", 45.3, "porcentaje", 0}
    };

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("1970-01-01T00:00:00Z"), std::string::npos);
    EXPECT_NE(resultado.find("45.3"), std::string::npos);
}

TEST(FormatterTextTest, ManejaSnapshotSinMetricas) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.timestamp = 0;

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("1970-01-01T00:00:00Z"), std::string::npos);
    EXPECT_NE(resultado.find("0.0"), std::string::npos);
}

TEST(FormatterTextTest, FormateaHistorialDeSnapshots) {
    FormatterText formatter;

    Snapshot primero{};
    primero.timestamp = 0;
    primero.metricas = {
        {"cpu.usage", 45.3, "porcentaje", 0}
    };

    Snapshot segundo{};
    segundo.timestamp = 1;
    segundo.metricas = {
        {"cpu.usage", 60.0, "porcentaje", 1}
    };

    std::vector<Snapshot> historial = {primero, segundo};

    std::string resultado = formatter.formatearHistorial(historial);

    EXPECT_NE(resultado.find("1970-01-01T00:00:00Z"), std::string::npos);
    EXPECT_NE(resultado.find("1970-01-01T00:00:01Z"), std::string::npos);
    EXPECT_NE(resultado.find("45.3"), std::string::npos);
    EXPECT_NE(resultado.find("60.0"), std::string::npos);
}

// Pruebas adicionales de casos borde

TEST(FormatterTextTest, IgnoraMetricasDesconocidas) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.metricas = {
        {"temperatura.cpu", 75.0, "celsius", 0},
        {"proceso.total", 10.0, "cantidad", 1}
    };

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("0.0"), std::string::npos);
    EXPECT_NE(resultado.find("0.0 B/0.0 B"), std::string::npos);
}

TEST(FormatterTextTest, FormateaCpuConValorCero) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.metricas = {
        {"cpu.usage", 0.0, "porcentaje", 0}
    };

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("0.0"), std::string::npos);
}

TEST(FormatterTextTest, FormateaCpuConValorNegativo) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.metricas = {
        {"cpu.usage", -12.5, "porcentaje", 0}
    };

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("-12.5"), std::string::npos);
}

TEST(FormatterTextTest, FormateaCpuSuperiorAlCienPorCiento) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.metricas = {
        {"cpu.usage", 150.5, "porcentaje", 0}
    };

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("150.5"), std::string::npos);
}

TEST(FormatterTextTest, HistorialVacioDevuelveCadenaVacia) {
    FormatterText formatter;

    std::vector<Snapshot> historial;

    std::string resultado = formatter.formatearHistorial(historial);

    EXPECT_TRUE(resultado.empty());
}

TEST(FormatterTextTest, ManejaMetricasIncompletas) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.metricas = {
        {"cpu.usage", 35.0, "porcentaje", 0},
        {"ram.used", 1024.0, "bytes", 1}
    };

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("35.0"), std::string::npos);
    EXPECT_NE(resultado.find("1.0 KB/0.0 B"), std::string::npos);
    EXPECT_NE(resultado.find("Disco"), std::string::npos);
    EXPECT_NE(resultado.find("Red"), std::string::npos);
}

TEST(FormatterTextTest, ConservaSignoDeValoresNegativosEnBytes) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.metricas = {
        {"ram.used", -1024.0, "bytes", 0},
        {"network.rx", -2048.0, "bytes/s", 1}
    };

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("-1.0 KB/0.0 B"), std::string::npos);
    EXPECT_NE(resultado.find("-2.0 KB/s/0.0 B/s"), std::string::npos);
}

TEST(FormatterTextTest, ConvierteValoresLimiteDeBytes) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.metricas = {
        {"ram.used", 1024.0, "bytes", 0},
        {"ram.total", 1048576.0, "bytes", 1}
    };

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("1.0 KB/1.0 MB"), std::string::npos);
}
