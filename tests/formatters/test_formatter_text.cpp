#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "core/types.hpp"
#include "formatters/formatter_text.hpp"

namespace {

pulso::core::Snapshot crearSnapshotEjemplo(std::int64_t timestamp) {
    pulso::core::Snapshot s;
    s.timestamp = timestamp;

    pulso::core::Metrica m1;
    m1.name = "cpu.usage";
    m1.unit = "percent";
    m1.value = 42.0;
    m1.timestamp = timestamp;

    s.metricas.push_back(m1);

    return s;
}

std::vector<std::string> obtenerLineas(const std::string& output) {
    std::vector<std::string> lineas;
    std::istringstream stream(output);
    std::string linea;

    while (std::getline(stream, linea)) {
        lineas.push_back(linea);
    }

    return lineas;
}

}

TEST(TestFormatterText, Test_FormatoYContentType) {
    pulso::formatters::FormatterText formatter;

    EXPECT_EQ(formatter.formato(), "text");
    EXPECT_EQ(formatter.contentType(), "text/plain");
}

TEST(TestFormatterText, Test_SnapshotVacio) {
    pulso::formatters::FormatterText formatter;

    pulso::core::Snapshot snapshot;
    snapshot.timestamp = 1000;

    std::string output = formatter.formatear(snapshot);
    EXPECT_FALSE(output.empty());
}

TEST(TestFormatterText, Test_SnapshotConMetricas) {
    pulso::formatters::FormatterText formatter;

    auto snapshot = crearSnapshotEjemplo(2000);
    std::string output = formatter.formatear(snapshot);

    EXPECT_NE(output.find("cpu.usage"), std::string::npos);
}

TEST(TestFormatterText, Test_HistorialConSnapshots) {
    pulso::formatters::FormatterText formatter;

    std::vector<pulso::core::Snapshot> historial;
    historial.push_back(crearSnapshotEjemplo(100));
    historial.push_back(crearSnapshotEjemplo(200));

    std::string output = formatter.formatearHistorial(historial);
    auto lineas = obtenerLineas(output);

    EXPECT_GT(lineas.size(), 0U);
}