#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <regex>
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

// --- Normalización de nombres (issue #1527) ---

namespace {

using chuspapulse::formatters::FormatterPrometheus;

const std::regex kNombrePrometheus("^[a-zA-Z_:][a-zA-Z0-9_:]*$");

} // namespace

TEST(TestFormatterPrometheus, Test_Normalizar_Puntos) {
    EXPECT_EQ(FormatterPrometheus::normalizarNombre("cpu.usage"),
              "chuspapulse_cpu_usage");
}

TEST(TestFormatterPrometheus, Test_Normalizar_Guiones) {
    EXPECT_EQ(FormatterPrometheus::normalizarNombre("disk.sda-1.used"),
              "chuspapulse_disk_sda_1_used");
}

TEST(TestFormatterPrometheus, Test_Normalizar_Espacios) {
    EXPECT_EQ(FormatterPrometheus::normalizarNombre("net rx bytes"),
              "chuspapulse_net_rx_bytes");
}

TEST(TestFormatterPrometheus, Test_Normalizar_NombreValidoNoCambia) {
    EXPECT_EQ(FormatterPrometheus::normalizarNombre("uptime_seconds"),
              "chuspapulse_uptime_seconds");
    EXPECT_EQ(FormatterPrometheus::normalizarNombre("Memory_Used2"),
              "chuspapulse_Memory_Used2");
}

TEST(TestFormatterPrometheus, Test_Normalizar_OtrosCaracteresYDosPuntos) {
    EXPECT_EQ(FormatterPrometheus::normalizarNombre("temp:cpu/0%"),
              "chuspapulse_temp_cpu_0_");
}

TEST(TestFormatterPrometheus, Test_Normalizar_EsDeterminista) {
    const std::string nombre = "network.eth-0.rx bytes";
    EXPECT_EQ(FormatterPrometheus::normalizarNombre(nombre),
              FormatterPrometheus::normalizarNombre(nombre));
}

TEST(TestFormatterPrometheus, Test_NombresExportadosCumplenSintaxis) {
    FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {
            {"cpu.usage", 1.0, "porcentaje", 1000},
            {"disk.sda-1.used", 2.0, "bytes", 1000},
            {"net rx", 3.0, "bytes", 1000},
            {"temp:cpu/0%", 4.0, "celsius", 1000},
            {"cpu.usage{core=\"0\"}", 5.0, "porcentaje", 1000},
        });

    for (const std::string& linea : obtenerLineas(formatter.formatear(snapshot))) {
        if (linea.empty()) {
            continue;
        }
        std::string nombre;
        if (linea.rfind("# HELP ", 0) == 0 || linea.rfind("# TYPE ", 0) == 0) {
            nombre = linea.substr(7, linea.find(' ', 7) - 7);
        } else {
            nombre = linea.substr(0, linea.find_first_of("{ "));
        }
        EXPECT_TRUE(std::regex_match(nombre, kNombrePrometheus))
            << "nombre inválido: '" << nombre << "' en la línea: " << linea;
    }
}

TEST(TestFormatterPrometheus, Test_ColisionNoSeFusionaEnSilencio) {
    FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {
            {"net.rx", 100.0, "bytes", 1000},
            {"net-rx", 200.0, "bytes", 1000},
            {"cpu.usage", 42.0, "porcentaje", 1000},
        });

    const std::vector<std::string> lineas =
        obtenerLineas(formatter.formatear(snapshot));

    std::vector<std::string> tipos;
    for (const std::string& linea : lineas) {
        if (linea.rfind("# TYPE ", 0) == 0) {
            tipos.push_back(linea.substr(7, linea.find(' ', 7) - 7));
        }
    }

    // Tres familias distintas: las dos que colisionan no se fusionan.
    ASSERT_EQ(tipos.size(), 3U);
    EXPECT_NE(tipos[0], tipos[1]);
    EXPECT_EQ(tipos[0].rfind("chuspapulse_net_rx_", 0), 0U);
    EXPECT_EQ(tipos[1].rfind("chuspapulse_net_rx_", 0), 0U);
    EXPECT_EQ(tipos[2], "chuspapulse_cpu_usage");

    // El HELP deja visible de qué nombre original viene cada una.
    const std::string output = formatter.formatear(snapshot);
    EXPECT_NE(output.find("(nombre original: net.rx)"), std::string::npos);
    EXPECT_NE(output.find("(nombre original: net-rx)"), std::string::npos);
}

TEST(TestFormatterPrometheus, Test_ColisionEscapaElNombreOriginalEnHelp) {
    FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {
            {"disk.c:\\", 1.0, "bytes", 1000},
            {"disk.c__", 2.0, "bytes", 1000},
        });

    const std::string output = formatter.formatear(snapshot);

    // La barra invertida se escapa y la salida sigue teniendo 2 HELP en
    // líneas propias (nada rompe el formato).
    EXPECT_NE(output.find("(nombre original: disk.c:\\\\)"), std::string::npos);
    const auto lineas = obtenerLineas(output);
    EXPECT_EQ(std::count_if(lineas.begin(), lineas.end(),
                            [](const std::string& l) {
                                return l.rfind("# HELP ", 0) == 0;
                            }),
              2);
}

TEST(TestFormatterPrometheus, Test_ColisionEsDeterministaSinImportarElOrden) {
    FormatterPrometheus formatter;

    const auto nombreDe = [&formatter](
                              const std::vector<chuspapulse::core::Metrica>& m,
                              const std::string& valor) {
        for (const std::string& linea :
             obtenerLineas(formatter.formatear(crearSnapshot(1000, m)))) {
            if (linea.rfind("chuspapulse_", 0) == 0 &&
                linea.substr(linea.find(' ') + 1) == valor) {
                return linea.substr(0, linea.find(' '));
            }
        }
        return std::string();
    };

    const chuspapulse::core::Metrica a{"net.rx", 1.0, "bytes", 1000};
    const chuspapulse::core::Metrica b{"net-rx", 2.0, "bytes", 1000};

    EXPECT_EQ(nombreDe({a, b}, "1"), nombreDe({b, a}, "1"));
    EXPECT_EQ(nombreDe({a, b}, "2"), nombreDe({b, a}, "2"));
    EXPECT_FALSE(nombreDe({a, b}, "1").empty());
}