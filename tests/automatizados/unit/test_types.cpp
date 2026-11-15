#include <gtest/gtest.h>

#include "core/types.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <string>

using namespace chuspapulse::core;

TEST(TestConstruccionMetrica, ValoresExplicitos) {
    Metrica metrica{
        "cpu.usage",
        75.5,
        "porcentaje",
        1710000000
    };

    EXPECT_EQ(metrica.nombre, "cpu.usage");
    EXPECT_DOUBLE_EQ(metrica.valor, 75.5);
    EXPECT_EQ(metrica.unidad, "porcentaje");
    EXPECT_EQ(metrica.timestamp, 1710000000);
}

TEST(TestConstruccionSnapshot, SnapshotConMetricas) {
    Metrica m1{
        "ram.used",
        2048,
        "MB",
        1710000000
    };

    Snapshot snapshot{
        1710000001,
        {m1}
    };

    EXPECT_EQ(snapshot.timestamp, 1710000001);
    EXPECT_EQ(snapshot.metricas.size(), 1);

    EXPECT_EQ(snapshot.metricas[0].nombre, "ram.used");
}

TEST(TestSnapshotVacio, SinMetricas) {
    Snapshot snapshot{
        1710000001,
        {}
    };

    EXPECT_EQ(snapshot.metricas.size(), 0);
}

TEST(TestAgregarMetricasASnapshot, IncrementoCorrecto) {
    Snapshot snapshot{
        1710000001,
        {}
    };

    snapshot.metricas.push_back({
        "cpu.usage",
        50.0,
        "%",
        1710000001
    });

    snapshot.metricas.push_back({
        "disk.used",
        80.0,
        "%",
        1710000001
    });

    EXPECT_EQ(snapshot.metricas.size(), 2);
}

TEST(TestCopiaDeSnapshot, CopiaIndependiente) {
    Snapshot original{
        1710000001,
        {
            {
                "cpu.usage",
                60.0,
                "%",
                1710000001
            }
        }
    };

    Snapshot copia = original;

    copia.metricas[0].valor = 99.0;

    EXPECT_DOUBLE_EQ(original.metricas[0].valor, 60.0);
    EXPECT_DOUBLE_EQ(copia.metricas[0].valor, 99.0);
}

TEST(TestTimestampCero, MetricaConTimestampCero) {
    Metrica metrica{
        "cpu.usage",
        10.0,
        "%",
        0
    };

    EXPECT_EQ(metrica.timestamp, 0);
    EXPECT_EQ(metrica.nombre, "cpu.usage");
    EXPECT_DOUBLE_EQ(metrica.valor, 10.0);
}

TEST(TestTimestampCero, SnapshotConTimestampCero) {
    Snapshot snapshot{
        0,
        {
            {"cpu.usage", 10.0, "%", 0}
        }
    };

    EXPECT_EQ(snapshot.timestamp, 0);
    ASSERT_EQ(snapshot.metricas.size(), 1u);
    EXPECT_EQ(snapshot.metricas[0].timestamp, 0);
}

TEST(TestBusquedaMetricas, RecuperarMetricaPorNombre) {
    Snapshot snapshot{
        1710000001,
        {
            {"cpu.usage", 45.0, "%", 1710000001},
            {"ram.used", 2048.0, "MB", 1710000001},
            {"disk.used", 70.0, "%", 1710000001}
        }
    };

    ASSERT_EQ(snapshot.metricas.size(), 3u);

    auto buscar = [&snapshot](const std::string& nombre) {
        return std::find_if(
            snapshot.metricas.begin(),
            snapshot.metricas.end(),
            [&nombre](const Metrica& m) { return m.nombre == nombre; });
    };

    auto ram = buscar("ram.used");
    ASSERT_NE(ram, snapshot.metricas.end());
    EXPECT_DOUBLE_EQ(ram->valor, 2048.0);
    EXPECT_EQ(ram->unidad, "MB");

    auto disco = buscar("disk.used");
    ASSERT_NE(disco, snapshot.metricas.end());
    EXPECT_DOUBLE_EQ(disco->valor, 70.0);

    auto cpu = buscar("cpu.usage");
    ASSERT_NE(cpu, snapshot.metricas.end());
    EXPECT_DOUBLE_EQ(cpu->valor, 45.0);

    EXPECT_EQ(buscar("net.rx"), snapshot.metricas.end());
}

TEST(TestValoresLimite, ValorNegativoSeAlmacenaSinCambios) {
    Metrica metrica{"cpu.usage", -5.0, "%", 1710000000};

    EXPECT_DOUBLE_EQ(metrica.valor, -5.0);
}

TEST(TestValoresLimite, ValorCeroSeAlmacenaSinCambios) {
    Metrica metrica{"cpu.usage", 0.0, "%", 1710000000};

    EXPECT_DOUBLE_EQ(metrica.valor, 0.0);
}

TEST(TestValoresLimite, ValorNaNSeConservaComoNaN) {
    Metrica metrica{"cpu.usage", std::numeric_limits<double>::quiet_NaN(), "%", 1710000000};

    EXPECT_TRUE(std::isnan(metrica.valor));
}

TEST(TestValoresLimite, ValorInfinitoSeConservaComoInfinito) {
    Metrica metrica{"cpu.usage", std::numeric_limits<double>::infinity(), "%", 1710000000};

    EXPECT_TRUE(std::isinf(metrica.valor));
    EXPECT_GT(metrica.valor, 0.0);
}

TEST(TestCamposVacios, NombreYUnidadVacios) {
    Metrica metrica{"", 10.0, "", 1710000000};

    EXPECT_TRUE(metrica.nombre.empty());
    EXPECT_TRUE(metrica.unidad.empty());
    EXPECT_DOUBLE_EQ(metrica.valor, 10.0);
}

TEST(TestTimestampsLimite, TimestampNegativo) {
    Metrica metrica{"cpu.usage", 10.0, "%", -1};

    EXPECT_EQ(metrica.timestamp, -1);
}

TEST(TestTimestampsLimite, TimestampMaximo) {
    const std::int64_t maximo = std::numeric_limits<std::int64_t>::max();
    Snapshot snapshot{
        maximo,
        {
            {"cpu.usage", 10.0, "%", maximo}
        }
    };

    EXPECT_EQ(snapshot.timestamp, maximo);
    ASSERT_EQ(snapshot.metricas.size(), 1u);
    EXPECT_EQ(snapshot.metricas[0].timestamp, maximo);
}

TEST(TestMetricasDuplicadas, NombresRepetidosSeConservan) {
    Snapshot snapshot{
        1710000001,
        {
            {"cpu.usage", 45.0, "%", 1710000001},
            {"cpu.usage", 55.0, "%", 1710000001}
        }
    };

    ASSERT_EQ(snapshot.metricas.size(), 2u);
    EXPECT_DOUBLE_EQ(snapshot.metricas[0].valor, 45.0);
    EXPECT_DOUBLE_EQ(snapshot.metricas[1].valor, 55.0);
}
