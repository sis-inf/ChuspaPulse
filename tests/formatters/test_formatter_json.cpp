#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "formatters/formatter_json.hpp"
#include "core/types.hpp"

using json = nlohmann::json;

static chuspapulse::core::Snapshot crearSnapshot(std::int64_t timestamp) {
    chuspapulse::core::Snapshot s;
    s.timestamp = timestamp;

    chuspapulse::core::Metrica m1;
    m1.nombre = "cpu.usage";
    m1.unidad = "percent";
    m1.valor = 42.0;
    m1.timestamp = timestamp;

    chuspapulse::core::Metrica m2;
    m2.nombre = "memory.used";
    m2.unidad = "bytes";
    m2.valor = 2048.0;
    m2.timestamp = timestamp;

    s.metricas.push_back(m1);
    s.metricas.push_back(m2);

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
    EXPECT_EQ(j["metricas"].size(), 0u);
}

TEST(TestFormatterJSON, Test_SnapshotConMetricas) {
    chuspapulse::formatters::FormatterJSON formatter;

    auto snapshot = crearSnapshot(2000);

    std::string output = formatter.formatear(snapshot);

    json j = json::parse(output);

    ASSERT_EQ(j["metricas"].size(), 2u);

    EXPECT_EQ(j["metricas"][0]["nombre"], "cpu.usage");
    EXPECT_EQ(j["metricas"][0]["unidad"], "percent");
    EXPECT_EQ(j["metricas"][1]["nombre"], "memory.used");
    EXPECT_EQ(j["metricas"][1]["unidad"], "bytes");
}

TEST(TestFormatterJSON, Test_HistorialVacio) {
    chuspapulse::formatters::FormatterJSON formatter;

    std::vector<chuspapulse::core::Snapshot> historial;

    std::string output = formatter.formatearHistorial(historial);

    json j = json::parse(output);

    EXPECT_EQ(j["total"], 0);
    EXPECT_TRUE(j["snapshots"].is_array());
    EXPECT_EQ(j["snapshots"].size(), 0u);
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
    EXPECT_EQ(j["snapshots"].size(), 3u);
}

TEST(TestFormatterJSON, Test_OutputEsParseable) {
    chuspapulse::formatters::FormatterJSON formatter;

    auto snapshot = crearSnapshot(5000);

    std::string output = formatter.formatear(snapshot);

    EXPECT_NO_THROW({
        json::parse(output);
    });
}
static chuspapulse::core::Metrica crearMetrica(
    const std::string& nombre,
    double valor,
    const std::string& unidad,
    std::int64_t timestamp) {
    chuspapulse::core::Metrica m;
    m.nombre = nombre;
    m.valor = valor;
    m.unidad = unidad;
    m.timestamp = timestamp;
    return m;
}

TEST(TestFormatterJSON, Test_CamposDeMetricaSeSerializan) {
    chuspapulse::formatters::FormatterJSON formatter;

    json j = json::parse(formatter.formatear(crearSnapshot(2000)));

    ASSERT_EQ(j["metricas"].size(), 2u);
    EXPECT_EQ(j["metricas"][0]["nombre"], "cpu.usage");
    EXPECT_DOUBLE_EQ(j["metricas"][0]["valor"].get<double>(), 42.0);
    EXPECT_EQ(j["metricas"][0]["unidad"], "percent");
    EXPECT_EQ(j["metricas"][0]["timestamp"].get<std::int64_t>(), 2000);
}

TEST(TestFormatterJSON, Test_ValoresNoFinitosSeSerializanComoNull) {
    chuspapulse::formatters::FormatterJSON formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 100;
    snapshot.metricas = {
        crearMetrica("a", std::numeric_limits<double>::quiet_NaN(), "u", 100),
        crearMetrica("b", std::numeric_limits<double>::infinity(), "u", 100),
        crearMetrica("c", -std::numeric_limits<double>::infinity(), "u", 100),
    };

    std::string output = formatter.formatear(snapshot);

    json j;
    ASSERT_NO_THROW(j = json::parse(output));
    ASSERT_EQ(j["metricas"].size(), 3u);
    EXPECT_TRUE(j["metricas"][0]["valor"].is_null());
    EXPECT_TRUE(j["metricas"][1]["valor"].is_null());
    EXPECT_TRUE(j["metricas"][2]["valor"].is_null());
}

TEST(TestFormatterJSON, Test_ValoresLimiteDeValor) {
    chuspapulse::formatters::FormatterJSON formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 200;
    snapshot.metricas = {
        crearMetrica("cero", 0.0, "u", 200),
        crearMetrica("negativo", -5.0, "u", 200),
        crearMetrica("maximo", std::numeric_limits<double>::max(), "u", 200),
    };

    json j = json::parse(formatter.formatear(snapshot));

    ASSERT_EQ(j["metricas"].size(), 3u);
    EXPECT_DOUBLE_EQ(j["metricas"][0]["valor"].get<double>(), 0.0);
    EXPECT_DOUBLE_EQ(j["metricas"][1]["valor"].get<double>(), -5.0);
    EXPECT_DOUBLE_EQ(j["metricas"][2]["valor"].get<double>(),
                     std::numeric_limits<double>::max());
}

TEST(TestFormatterJSON, Test_TimestampsLimite) {
    chuspapulse::formatters::FormatterJSON formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = std::numeric_limits<std::int64_t>::max();
    snapshot.metricas = {
        crearMetrica("m", 1.0, "u", std::numeric_limits<std::int64_t>::min()),
    };

    json j = json::parse(formatter.formatear(snapshot));

    EXPECT_EQ(j["timestamp"].get<std::int64_t>(),
              std::numeric_limits<std::int64_t>::max());
    ASSERT_EQ(j["metricas"].size(), 1u);
    EXPECT_EQ(j["metricas"][0]["timestamp"].get<std::int64_t>(),
              std::numeric_limits<std::int64_t>::min());
}

TEST(TestFormatterJSON, Test_CaracteresEspecialesSeEscapan) {
    chuspapulse::formatters::FormatterJSON formatter;

    const std::string nombre = "a\"b\\c\nd\te";
    const std::string unidad = "\xC2\xB0" "C";

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 300;
    snapshot.metricas = {crearMetrica(nombre, 1.0, unidad, 300)};

    std::string output = formatter.formatear(snapshot);

    EXPECT_EQ(output.find('\n'), std::string::npos);

    json j;
    ASSERT_NO_THROW(j = json::parse(output));
    ASSERT_EQ(j["metricas"].size(), 1u);
    EXPECT_EQ(j["metricas"][0]["nombre"].get<std::string>(), nombre);
    EXPECT_EQ(j["metricas"][0]["unidad"].get<std::string>(), unidad);
}

TEST(TestFormatterJSON, Test_NombreYUnidadVacios) {
    chuspapulse::formatters::FormatterJSON formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 400;
    snapshot.metricas = {crearMetrica("", 7.0, "", 400)};

    json j = json::parse(formatter.formatear(snapshot));

    ASSERT_EQ(j["metricas"].size(), 1u);
    EXPECT_EQ(j["metricas"][0]["nombre"].get<std::string>(), "");
    EXPECT_EQ(j["metricas"][0]["unidad"].get<std::string>(), "");
}

TEST(TestFormatterJSON, Test_Utf8InvalidoLanzaExcepcion) {
    chuspapulse::formatters::FormatterJSON formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 500;
    snapshot.metricas = {crearMetrica("\xFF\xFE", 1.0, "u", 500)};

    EXPECT_THROW(formatter.formatear(snapshot), json::type_error);
}

TEST(TestFormatterJSON, Test_HistorialConSnapshotSinMetricas) {
    chuspapulse::formatters::FormatterJSON formatter;

    chuspapulse::core::Snapshot vacio;
    vacio.timestamp = 10;

    std::vector<chuspapulse::core::Snapshot> historial = {vacio, crearSnapshot(20)};

    json j = json::parse(formatter.formatearHistorial(historial));

    EXPECT_EQ(j["total"].get<std::size_t>(), 2u);
    ASSERT_EQ(j["snapshots"].size(), 2u);
    EXPECT_TRUE(j["snapshots"][0]["metricas"].is_array());
    EXPECT_EQ(j["snapshots"][0]["metricas"].size(), 0u);
    EXPECT_EQ(j["snapshots"][1]["metricas"].size(), 2u);
}

TEST(TestFormatterJSON, Test_HistorialConservaElOrden) {
    chuspapulse::formatters::FormatterJSON formatter;

    std::vector<chuspapulse::core::Snapshot> historial = {
        crearSnapshot(300), crearSnapshot(100), crearSnapshot(200)};

    json j = json::parse(formatter.formatearHistorial(historial));

    ASSERT_EQ(j["snapshots"].size(), 3u);
    EXPECT_EQ(j["snapshots"][0]["timestamp"].get<std::int64_t>(), 300);
    EXPECT_EQ(j["snapshots"][1]["timestamp"].get<std::int64_t>(), 100);
    EXPECT_EQ(j["snapshots"][2]["timestamp"].get<std::int64_t>(), 200);
}

TEST(TestFormatterJSON, Test_HistorialConValoresNoFinitos) {
    chuspapulse::formatters::FormatterJSON formatter;

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = 600;
    snapshot.metricas = {
        crearMetrica("nan", std::numeric_limits<double>::quiet_NaN(), "u", 600),
    };

    std::string output = formatter.formatearHistorial({snapshot});

    json j;
    ASSERT_NO_THROW(j = json::parse(output));
    EXPECT_EQ(j["total"].get<std::size_t>(), 1u);
    EXPECT_TRUE(j["snapshots"][0]["metricas"][0]["valor"].is_null());
}

TEST(TestFormatterJSON, Test_HistorialGrande) {
    chuspapulse::formatters::FormatterJSON formatter;

    std::vector<chuspapulse::core::Snapshot> historial;
    for (int i = 0; i < 1000; ++i) {
        historial.push_back(crearSnapshot(i));
    }

    json j = json::parse(formatter.formatearHistorial(historial));

    EXPECT_EQ(j["total"].get<std::size_t>(), 1000u);
    EXPECT_EQ(j["snapshots"].size(), 1000u);
}