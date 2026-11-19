#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "formatters/formatter_json.hpp"
#include "core/types.hpp"

using json = nlohmann::json;

static chuspapulse::core::Snapshot crearSnapshot(std::int64_t timestamp) {
    chuspapulse::core::Snapshot s;
    s.timestamp = timestamp;

    chuspapulse::core::Metrica m1;
    m1.name = "cpu.usage";
    m1.unit = "percent";
    m1.value = 42.0;
    m1.timestamp = timestamp;

    chuspapulse::core::Metrica m2;
    m2.name = "memory.used";
    m2.unit = "bytes";
    m2.value = 2048.0;
    m2.timestamp = timestamp;

    s.metrics.push_back(m1);
    s.metrics.push_back(m2);

    return s;
}

TEST(TestFormatterJSON, Test_FormatoYContentType) {
    chuspapulse::formatters::FormatterJSON formatter;

    EXPECT_EQ(formatter.formato(), "json");
    EXPECT_EQ(formatter.contentType(), "application/json");
}

TEST(TestFormatterJSON, Test_SnapshotVacio) {
    chuspapulse::formatters::FormatterJSON formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 1000;

    std::string output = formatter.formatear(snapshot);

    json j = json::parse(output);

    EXPECT_TRUE(j.contains("timestamp"));
    EXPECT_TRUE(j.contains("metricas"));
    EXPECT_TRUE(j["metricas"].is_array());
    EXPECT_EQ(j["metricas"].size(), 0);
}

TEST(TestFormatterJSON, Test_SnapshotConMetricas) {
    chuspapulse::formatters::FormatterJSON formatter;

    auto snapshot = crearSnapshot(2000);

    std::string output = formatter.formatear(snapshot);

    json j = json::parse(output);

    ASSERT_EQ(j["metricas"].size(), 2);

    EXPECT_EQ(j["metricas"][0]["name"], "cpu.usage");
    EXPECT_EQ(j["metricas"][0]["unit"], "percent");

    EXPECT_EQ(j["metricas"][1]["name"], "memory.used");
    EXPECT_EQ(j["metricas"][1]["unit"], "bytes");
}

TEST(TestFormatterJSON, Test_HistorialVacio) {
    chuspapulse::formatters::FormatterJSON formatter;

    std::vector<chuspapulse::core::Snapshot> historial;

    std::string output = formatter.formatearHistorial(historial);

    json j = json::parse(output);

    EXPECT_EQ(j["total"], 0);
    EXPECT_TRUE(j["snapshots"].is_array());
    EXPECT_EQ(j["snapshots"].size(), 0);
}

TEST(TestFormatterJSON, Test_HistorialConSnapshots) {
    chuspapulse::formatters::FormatterJSON formatter;

    std::vector<chuspapulse::core::Snapshot> historial;

    historial.push_back(crearSnapshot(100));
    historial.push_back(crearSnapshot(200));
    historial.push_back(crearSnapshot(300));

    std::string output = formatter.formatearHistorial(historial);

    json j = json::parse(output);

    EXPECT_EQ(j["total"], 3);
    EXPECT_EQ(j["snapshots"].size(), 3);
}

TEST(TestFormatterJSON, Test_OutputEsParseable) {
    chuspapulse::formatters::FormatterJSON formatter;

    auto snapshot = crearSnapshot(5000);

    std::string output = formatter.formatear(snapshot);

    EXPECT_NO_THROW({
        json::parse(output);
    });
}