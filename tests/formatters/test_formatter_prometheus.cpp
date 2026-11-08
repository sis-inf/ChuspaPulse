#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "core/types.hpp"
#include "formatters/formatter_prometheus.hpp"

namespace {

chuspapulse::core::Snapshot crearSnapshot(
    std::int64_t timestamp,
    const std::vector<chuspapulse::core::Metrica>& metricas) {
    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = timestamp;
    snapshot.metricas = metricas;
    return snapshot;
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

} // namespace

TEST(TestFormatterPrometheus, Test_FormatoYContentType) {
    chuspapulse::formatters::FormatterPrometheus formatter;

    EXPECT_EQ(formatter.formato(), "prometheus");
    EXPECT_EQ(formatter.contentType(), "text/plain; version=0.0.4");
}

TEST(TestFormatterPrometheus, Test_TransformacionDeNombres) {
    chuspapulse::formatters::FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {{"cpu.usage", 42.0, "porcentaje", 1000}});

    const std::string output = formatter.formatear(snapshot);

    EXPECT_NE(output.find("chuspapulse_cpu_usage"), std::string::npos);
}

TEST(TestFormatterPrometheus, Test_EstructuraDeLineas) {
    chuspapulse::formatters::FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {{"cpu.usage", 42.0, "porcentaje", 1000}});

    const std::vector<std::string> lineas =
        obtenerLineas(formatter.formatear(snapshot));

    ASSERT_EQ(lineas.size(), 3U);
    EXPECT_EQ(lineas[0].find("# HELP "), 0U);
    EXPECT_EQ(lineas[1].find("# TYPE "), 0U);
    EXPECT_EQ(lineas[2].find("chuspapulse_cpu_usage "), 0U);
}

TEST(TestFormatterPrometheus, Test_TipoSiempreGauge) {
    chuspapulse::formatters::FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {
            {"cpu.usage", 42.0, "porcentaje", 1000},
            {"memory.used", 2048.0, "bytes", 1000},
        });

    const std::vector<std::string> lineas =
        obtenerLineas(formatter.formatear(snapshot));

    std::size_t lineasTipo = 0;

    for (const std::string& linea : lineas) {
        if (linea.find("# TYPE ") == 0U) {
            ++lineasTipo;
            EXPECT_NE(linea.find(" gauge"), std::string::npos);
        }
    }

    EXPECT_EQ(lineasTipo, 2U);
}

TEST(TestFormatterPrometheus, Test_SnapshotVacio) {
    chuspapulse::formatters::FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(1000, {});

    const std::string output = formatter.formatear(snapshot);

    EXPECT_TRUE(std::all_of(
        output.begin(),
        output.end(),
        [](unsigned char caracter) {
            return std::isspace(caracter) != 0;
        }));
}

TEST(TestFormatterPrometheus, Test_HistorialVacioDevuelveStringVacio) {
    chuspapulse::formatters::FormatterPrometheus formatter;

    EXPECT_EQ(formatter.formatearHistorial({}), "");
}

TEST(TestFormatterPrometheus, Test_HistorialConSnapshots_DevuelveSoloElUltimo) {
    chuspapulse::formatters::FormatterPrometheus formatter;

    const auto s1 = crearSnapshot(
        100,
        {{"cpu.usage", 10.0, "porcentaje", 100}});

    const auto s2 = crearSnapshot(
        200,
        {{"cpu.usage", 20.0, "porcentaje", 200}});

    const auto s3 = crearSnapshot(
        300,
        {{"cpu.usage", 30.0, "porcentaje", 300}});

    EXPECT_EQ(
        formatter.formatearHistorial({s1, s2, s3}),
        formatter.formatear(s3));
}

TEST(TestFormatterPrometheus, Test_VariasInstanciasMismaMetrica_UnSoloHelpYType) {
    chuspapulse::formatters::FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {
            {"cpu.usage{core=\"0\"}", 10.5, "porcentaje", 1000},
            {"cpu.usage{core=\"1\"}", 20.5, "porcentaje", 1000},
            {"cpu.usage{core=\"2\"}", 30.5, "porcentaje", 1000},
        });

    const std::vector<std::string> lineas =
        obtenerLineas(formatter.formatear(snapshot));

    const auto contar = [&lineas](const std::string& prefijo) {
        return std::count_if(
            lineas.begin(), lineas.end(),
            [&prefijo](const std::string& linea) {
                return linea.find(prefijo) == 0U;
            });
    };

    EXPECT_EQ(contar("# HELP chuspapulse_cpu_usage "), 1);
    EXPECT_EQ(contar("# TYPE chuspapulse_cpu_usage "), 1);

    const std::vector<std::string> esperado = {
        "# HELP chuspapulse_cpu_usage Metrica de chuspapulse",
        "# TYPE chuspapulse_cpu_usage gauge",
        "chuspapulse_cpu_usage{core=\"0\"} 10.5",
        "chuspapulse_cpu_usage{core=\"1\"} 20.5",
        "chuspapulse_cpu_usage{core=\"2\"} 30.5",
    };

    EXPECT_EQ(lineas, esperado);
}

TEST(TestFormatterPrometheus, Test_MetricasIntercaladas_SeAgrupanPorNombre) {
    chuspapulse::formatters::FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {
            {"net.rx", 100.0, "bytes", 1000},
            {"cpu.usage", 42.0, "porcentaje", 1000},
            {"net.rx", 200.0, "bytes", 1000},
        });

    const std::vector<std::string> lineas =
        obtenerLineas(formatter.formatear(snapshot));

    const std::vector<std::string> esperado = {
        "# HELP chuspapulse_net_rx Metrica de chuspapulse",
        "# TYPE chuspapulse_net_rx gauge",
        "chuspapulse_net_rx 100",
        "chuspapulse_net_rx 200",
        "",
        "# HELP chuspapulse_cpu_usage Metrica de chuspapulse",
        "# TYPE chuspapulse_cpu_usage gauge",
        "chuspapulse_cpu_usage 42",
    };

    EXPECT_EQ(lineas, esperado);
}