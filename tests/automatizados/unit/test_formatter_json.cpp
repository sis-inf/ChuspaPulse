
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <limits>

#include "core/types.hpp"
#include "formatters/formatter_json.hpp"

using json = nlohmann::json;

namespace {

chuspapulse::core::Snapshot crearSnapshotConocido()
{
    chuspapulse::core::Snapshot snapshot;

    snapshot.timestamp = 1710000000;

    snapshot.metricas = {
        {"cpu", 42.5, "porcentaje", 1710000000},
        {"memory", 68.0, "porcentaje", 1710000000},
        {"disk", 71.3, "porcentaje", 1710000000},
        {"network", 2048.0, "bytes", 1710000000}
    };

    return snapshot;
}

}

TEST(FormatterJSONTest, ResultadoNoEsVacio)
{
    auto snapshot = crearSnapshotConocido();
    chuspapulse::formatters::FormatterJSON formatter;

    EXPECT_FALSE(formatter.formatear(snapshot).empty());
}

TEST(FormatterJSONTest, ContieneCamposRequeridos)
{
    auto snapshot = crearSnapshotConocido();
    chuspapulse::formatters::FormatterJSON formatter;

    json resultado = json::parse(formatter.formatear(snapshot));

    EXPECT_TRUE(resultado.contains("timestamp"));
    EXPECT_TRUE(resultado.contains("metricas"));
    EXPECT_EQ(resultado["metricas"].size(), 4);
}

TEST(FormatterJSONTest, SnapshotSinMetricas)
{
    chuspapulse::core::Snapshot snapshot{};
    snapshot.timestamp = 1000;

    chuspapulse::formatters::FormatterJSON formatter;

    json resultado = json::parse(formatter.formatear(snapshot));

    EXPECT_EQ(resultado["timestamp"], 1000);
    EXPECT_TRUE(resultado["metricas"].is_array());
    EXPECT_TRUE(resultado["metricas"].empty());
}

TEST(FormatterJSONTest, ValorCero)
{
    auto snapshot = crearSnapshotConocido();
    snapshot.metricas = {
        {"cpu", 0.0, "porcentaje", 1000}
    };

    chuspapulse::formatters::FormatterJSON formatter;
    json resultado = json::parse(formatter.formatear(snapshot));

    EXPECT_DOUBLE_EQ(resultado["metricas"][0]["valor"], 0.0);
}

TEST(FormatterJSONTest, ValorNegativo)
{
    auto snapshot = crearSnapshotConocido();
    snapshot.metricas = {
        {"temperatura", -15.5, "celsius", 1000}
    };

    chuspapulse::formatters::FormatterJSON formatter;
    json resultado = json::parse(formatter.formatear(snapshot));

    EXPECT_DOUBLE_EQ(resultado["metricas"][0]["valor"], -15.5);
}

TEST(FormatterJSONTest, ValorSuperiorAlCienPorCiento)
{
    auto snapshot = crearSnapshotConocido();
    snapshot.metricas = {
        {"cpu", 150.0, "porcentaje", 1000}
    };

    chuspapulse::formatters::FormatterJSON formatter;
    json resultado = json::parse(formatter.formatear(snapshot));

    EXPECT_DOUBLE_EQ(resultado["metricas"][0]["valor"], 150.0);
}

TEST(FormatterJSONTest, ValorNumericoMuyGrande)
{
    auto snapshot = crearSnapshotConocido();
    double valor = std::numeric_limits<double>::max();

    snapshot.metricas = {
        {"memory", valor, "bytes", 1000}
    };

    chuspapulse::formatters::FormatterJSON formatter;
    json resultado = json::parse(formatter.formatear(snapshot));

    EXPECT_DOUBLE_EQ(resultado["metricas"][0]["valor"], valor);
}

TEST(FormatterJSONTest, MetricasConCamposVacios)
{
    auto snapshot = crearSnapshotConocido();
    snapshot.metricas = {
        {"", 10.0, "", 1000}
    };

    chuspapulse::formatters::FormatterJSON formatter;
    json resultado = json::parse(formatter.formatear(snapshot));

    EXPECT_EQ(resultado["metricas"][0]["nombre"], "");
    EXPECT_EQ(resultado["metricas"][0]["unidad"], "");
}

TEST(FormatterJSONTest, ValoresNoFinitosSeRepresentanComoNull)
{
    auto snapshot = crearSnapshotConocido();

    snapshot.metricas = {
        {"infinito", std::numeric_limits<double>::infinity(), "bytes", 1000},
        {"nan", std::numeric_limits<double>::quiet_NaN(), "bytes", 1000}
    };

    chuspapulse::formatters::FormatterJSON formatter;
    json resultado = json::parse(formatter.formatear(snapshot));

    EXPECT_TRUE(resultado["metricas"][0]["valor"].is_null());
    EXPECT_TRUE(resultado["metricas"][1]["valor"].is_null());
}

TEST(FormatterJSONTest, TimestampsLimite)
{
    auto snapshot = crearSnapshotConocido();

    snapshot.timestamp = std::numeric_limits<std::int64_t>::max();
    snapshot.metricas = {
        {"cpu", 50.0, "porcentaje",
         std::numeric_limits<std::int64_t>::min()}
    };

    chuspapulse::formatters::FormatterJSON formatter;
    json resultado = json::parse(formatter.formatear(snapshot));

    EXPECT_EQ(
        resultado["timestamp"].get<std::int64_t>(),
        std::numeric_limits<std::int64_t>::max()
    );

    EXPECT_EQ(
        resultado["metricas"][0]["timestamp"].get<std::int64_t>(),
        std::numeric_limits<std::int64_t>::min()
    );
}

TEST(FormatterJSONTest, HistorialVacio)
{
    chuspapulse::formatters::FormatterJSON formatter;

    std::vector<chuspapulse::core::Snapshot> historial;

    json resultado = json::parse(formatter.formatearHistorial(historial));

    EXPECT_EQ(resultado["total"], 0);
    EXPECT_TRUE(resultado["snapshots"].is_array());
    EXPECT_TRUE(resultado["snapshots"].empty());
}

TEST(FormatterJSONTest, HistorialConVariosSnapshots)
{
    chuspapulse::formatters::FormatterJSON formatter;

    std::vector<chuspapulse::core::Snapshot> historial;

    historial.push_back(crearSnapshotConocido());
    historial.push_back(crearSnapshotConocido());
    historial.push_back(crearSnapshotConocido());

    json resultado = json::parse(formatter.formatearHistorial(historial));

    EXPECT_EQ(resultado["total"], 3);
    EXPECT_EQ(resultado["snapshots"].size(), 3);
}

TEST(FormatterJSONTest, ResultadoEsJSONValido)
{
    auto snapshot = crearSnapshotConocido();
    chuspapulse::formatters::FormatterJSON formatter;

    json resultado;

    EXPECT_NO_THROW({
        resultado = json::parse(formatter.formatear(snapshot));
    });

    EXPECT_TRUE(resultado.is_object());
}
