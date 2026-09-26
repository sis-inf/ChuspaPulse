#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "core/types.hpp"
#include "formatters/formatter_csv.hpp"

namespace {

pulso::core::Snapshot crearSnapshotEjemplo(std::int64_t timestamp) {
    pulso::core::Snapshot s;
    s.timestamp = timestamp;

    pulso::core::Metrica m1;
    m1.name = "cpu.usage";
    m1.unit = "percent";
    m1.value = 42.0;
    m1.timestamp = timestamp;

    pulso::core::Metrica m2;
    m2.name = "memory.used";
    m2.unit = "bytes";
    m2.value = 2048.0;
    m2.timestamp = timestamp;

    s.metricas.push_back(m1);
    s.metricas.push_back(m2);

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

TEST(TestFormatterCSV, Test_FormatoYContentType) {
    pulso::formatters::FormatterCSV formatter;

    EXPECT_EQ(formatter.formato(), "csv");
    EXPECT_EQ(formatter.contentType(), "text/csv");
}

TEST(TestFormatterCSV, Test_SnapshotVacio) {
    pulso::formatters::FormatterCSV formatter;

    pulso::core::Snapshot snapshot;
    snapshot.timestamp = 1000;

    std::string output = formatter.formatear(snapshot);
    auto lineas = obtenerLineas(output);

    ASSERT_EQ(lineas.size(), 1U);
    EXPECT_NE(output.find("timestamp"), std::string::npos);
}

TEST(TestFormatterCSV, Test_SnapshotConMetricas) {
    pulso::formatters::FormatterCSV formatter;

    auto snapshot = crearSnapshotEjemplo(2000);
    std::string output = formatter.formatear(snapshot);
    auto lineas = obtenerLineas(output);

    ASSERT_EQ(lineas.size(), 3U);
    EXPECT_NE(lineas[1].find("cpu.usage"), std::string::npos);
    EXPECT_NE(lineas[2].find("memory.used"), std::string::npos);
}

TEST(TestFormatterCSV, Test_HistorialConSnapshots) {
    pulso::formatters::FormatterCSV formatter;

    std::vector<pulso::core::Snapshot> historial;
    historial.push_back(crearSnapshotEjemplo(100));
    historial.push_back(crearSnapshotEjemplo(200));

    std::string output = formatter.formatearHistorial(historial);
    auto lineas = obtenerLineas(output);

    ASSERT_EQ(lineas.size(), 5U);
}