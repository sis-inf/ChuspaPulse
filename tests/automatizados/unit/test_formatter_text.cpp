#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "formatters/formatter_text.hpp"
#include "core/types.hpp"

using chuspapulse::core::Metrica;
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

    EXPECT_NE(
        resultado.find("1970-01-01T00:00:00Z"),
        std::string::npos
    );

    EXPECT_NE(
        resultado.find("CPU: 45.3%"),
        std::string::npos
    );
}

TEST(FormatterTextTest, ManejaSnapshotSinMetricas) {
    FormatterText formatter;

    Snapshot snapshot{};
    snapshot.timestamp = 0;

    std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(
        resultado.find("1970-01-01T00:00:00Z"),
        std::string::npos
    );

    EXPECT_NE(
        resultado.find("CPU: 0.0%"),
        std::string::npos
    );
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

    EXPECT_NE(
        resultado.find("1970-01-01T00:00:00Z"),
        std::string::npos
    );

    EXPECT_NE(
        resultado.find("1970-01-01T00:00:01Z"),
        std::string::npos
    );

    EXPECT_NE(
        resultado.find("CPU: 45.3%"),
        std::string::npos
    );

    EXPECT_NE(
        resultado.find("CPU: 60.0%"),
        std::string::npos
    );
}
