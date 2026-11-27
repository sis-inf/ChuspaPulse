#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <set>

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

// ---------------------------------------------------------------------------
// Casos borde adicionales
// ---------------------------------------------------------------------------

namespace {

std::size_t contarPrefijo(const std::string& salida, const std::string& prefijo) {
    std::size_t total = 0;
    for (const std::string& linea : obtenerLineas(salida)) {
        if (linea.rfind(prefijo, 0) == 0) {
            ++total;
        }
    }
    return total;
}

std::vector<std::string> nombresDeTipo(const std::string& salida) {
    std::vector<std::string> nombres;
    for (const std::string& linea : obtenerLineas(salida)) {
        if (linea.rfind("# TYPE ", 0) == 0) {
            nombres.push_back(linea.substr(7, linea.find(' ', 7) - 7));
        }
    }
    return nombres;
}

} // namespace

// --- normalizarNombre ---

TEST(TestFormatterPrometheus, Test_Normalizar_NombreVacio) {
    EXPECT_EQ(FormatterPrometheus::normalizarNombre(""), "chuspapulse_");
}

TEST(TestFormatterPrometheus, Test_Normalizar_BytesNoAscii_UnGuionBajoPorByte) {
    // 'ñ' en UTF-8 ocupa 2 bytes (0xC3 0xB1): cada byte se reemplaza.
    EXPECT_EQ(FormatterPrometheus::normalizarNombre("a\xC3\xB1" "b"),
              "chuspapulse_a__b");
}

TEST(TestFormatterPrometheus, Test_Normalizar_EmpiezaConDigito_QuedaValido) {
    const std::string nombre = FormatterPrometheus::normalizarNombre("9cpu.usage");
    EXPECT_EQ(nombre, "chuspapulse_9cpu_usage");
    EXPECT_TRUE(std::regex_match(nombre, kNombrePrometheus));
}

TEST(TestFormatterPrometheus, Test_Normalizar_SaltoDeLineaYTabulador) {
    EXPECT_EQ(FormatterPrometheus::normalizarNombre("a\nb\tc"),
              "chuspapulse_a_b_c");
}

TEST(TestFormatterPrometheus, Test_Normalizar_NombreMuyLargo) {
    const std::string largo(5000, 'x');
    const std::string nombre = FormatterPrometheus::normalizarNombre(largo);
    EXPECT_EQ(nombre.size(), 12U + 5000U);
    EXPECT_EQ(nombre.rfind("chuspapulse_", 0), 0U);
}

// --- formatear: estructura exacta ---

TEST(TestFormatterPrometheus, Test_SnapshotVacio_DevuelveCadenaVacia) {
    FormatterPrometheus formatter;
    EXPECT_EQ(formatter.formatear(crearSnapshot(1000, {})), "");
}

TEST(TestFormatterPrometheus, Test_SalidaExacta_UnaMetrica) {
    FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000, {{"cpu.usage", 42.0, "porcentaje", 1000}});

    EXPECT_EQ(formatter.formatear(snapshot),
              "# HELP chuspapulse_cpu_usage Metrica de chuspapulse\n"
              "# TYPE chuspapulse_cpu_usage gauge\n"
              "chuspapulse_cpu_usage 42\n");
}

TEST(TestFormatterPrometheus, Test_SalidaExacta_UnaLineaEnBlancoEntreFamilias) {
    FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {
            {"cpu.usage", 42.0, "porcentaje", 1000},
            {"memory.used", 2048.0, "bytes", 1000},
        });

    EXPECT_EQ(formatter.formatear(snapshot),
              "# HELP chuspapulse_cpu_usage Metrica de chuspapulse\n"
              "# TYPE chuspapulse_cpu_usage gauge\n"
              "chuspapulse_cpu_usage 42\n"
              "\n"
              "# HELP chuspapulse_memory_used Metrica de chuspapulse\n"
              "# TYPE chuspapulse_memory_used gauge\n"
              "chuspapulse_memory_used 2048\n");
}

TEST(TestFormatterPrometheus, Test_NombreVacio_UsaSoloElPrefijo) {
    FormatterPrometheus formatter;

    const std::vector<std::string> esperado = {
        "# HELP chuspapulse_ Metrica de chuspapulse",
        "# TYPE chuspapulse_ gauge",
        "chuspapulse_ 1",
    };

    EXPECT_EQ(obtenerLineas(formatter.formatear(
                  crearSnapshot(1000, {{"", 1.0, "u", 1000}}))),
              esperado);
}

TEST(TestFormatterPrometheus, Test_NombreConSaltoDeLinea_NoRompeLaSalida) {
    FormatterPrometheus formatter;

    const auto lineas = obtenerLineas(formatter.formatear(
        crearSnapshot(1000, {{"cpu\nusage", 1.0, "u", 1000}})));

    ASSERT_EQ(lineas.size(), 3U);
    EXPECT_EQ(lineas[2], "chuspapulse_cpu_usage 1");
}

TEST(TestFormatterPrometheus, Test_Formatear_EsIdempotente) {
    FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {
            {"cpu.usage{core=\"0\"}", 1.0, "porcentaje", 1000},
            {"net.rx", 2.0, "bytes", 1000},
        });

    EXPECT_EQ(formatter.formatear(snapshot), formatter.formatear(snapshot));
}

TEST(TestFormatterPrometheus, Test_TimestampYUnidadNoAparecenEnLaSalida) {
    FormatterPrometheus formatter;

    const std::string salida = formatter.formatear(
        crearSnapshot(555, {{"x", 7.0, "unidad_rara", 987654321}}));

    EXPECT_EQ(salida.find("987654321"), std::string::npos);
    EXPECT_EQ(salida.find("unidad_rara"), std::string::npos);
    EXPECT_EQ(salida.find("555"), std::string::npos);
}

// --- formatear: valores ---

TEST(TestFormatterPrometheus, Test_Valores_CeroNegativoYDecimal) {
    FormatterPrometheus formatter;

    const std::string salida = formatter.formatear(crearSnapshot(
        1000,
        {
            {"m.zero", 0.0, "u", 1000},
            {"m.neg", -5.5, "u", 1000},
            {"m.dec", 0.25, "u", 1000},
        }));

    EXPECT_NE(salida.find("chuspapulse_m_zero 0\n"), std::string::npos);
    EXPECT_NE(salida.find("chuspapulse_m_neg -5.5\n"), std::string::npos);
    EXPECT_NE(salida.find("chuspapulse_m_dec 0.25\n"), std::string::npos);
}

TEST(TestFormatterPrometheus, Test_Valores_PrecisionDe15Digitos) {
    FormatterPrometheus formatter;

    const std::string salida = formatter.formatear(crearSnapshot(
        1000,
        {
            {"p.suma", 0.1 + 0.2, "u", 1000},
            {"p.grande", 1234567.891, "u", 1000},
        }));

    EXPECT_NE(salida.find("chuspapulse_p_suma 0.3\n"), std::string::npos);
    EXPECT_NE(salida.find("chuspapulse_p_grande 1234567.891\n"),
              std::string::npos);
}

TEST(TestFormatterPrometheus, Test_Valores_MuyGrandeUsaNotacionCientifica) {
    FormatterPrometheus formatter;

    const std::string salida = formatter.formatear(
        crearSnapshot(1000, {{"m", 1e20, "u", 1000}}));

    EXPECT_NE(salida.find("chuspapulse_m 1e+"), std::string::npos);
}

TEST(TestFormatterPrometheus, Test_Valores_NaNEInfinito_NoLanzanYConservanElNombre) {
    FormatterPrometheus formatter;

    const auto snapshot = crearSnapshot(
        1000,
        {
            {"m.nan", std::numeric_limits<double>::quiet_NaN(), "u", 1000},
            {"m.inf", std::numeric_limits<double>::infinity(), "u", 1000},
            {"m.ninf", -std::numeric_limits<double>::infinity(), "u", 1000},
        });

    std::string salida;
    EXPECT_NO_THROW(salida = formatter.formatear(snapshot));

    EXPECT_NE(salida.find("chuspapulse_m_nan "), std::string::npos);
    EXPECT_NE(salida.find("chuspapulse_m_inf "), std::string::npos);
    EXPECT_NE(salida.find("chuspapulse_m_ninf "), std::string::npos);
    EXPECT_EQ(contarPrefijo(salida, "# TYPE "), 3U);
}

TEST(TestFormatterPrometheus, Test_MuchasMetricas_UnaFamiliaPorNombre) {
    FormatterPrometheus formatter;

    std::vector<chuspapulse::core::Metrica> metricas;
    for (int i = 0; i < 500; ++i) {
        metricas.push_back({"m." + std::to_string(i),
                            static_cast<double>(i), "u", 1000});
    }

    const std::string salida = formatter.formatear(crearSnapshot(1000, metricas));

    EXPECT_EQ(contarPrefijo(salida, "# HELP "), 500U);
    EXPECT_EQ(contarPrefijo(salida, "# TYPE "), 500U);
    // 3 líneas por familia + 1 línea en blanco entre familias.
    EXPECT_EQ(obtenerLineas(salida).size(), 500U * 3U + 499U);
}

// --- formatearHistorial ---

TEST(TestFormatterPrometheus, Test_Historial_UnSoloSnapshot_IgualAFormatear) {
    FormatterPrometheus formatter;

    const auto s = crearSnapshot(
        100, {{"cpu.usage", 10.0, "porcentaje", 100}});

    EXPECT_EQ(formatter.formatearHistorial({s}), formatter.formatear(s));
}

TEST(TestFormatterPrometheus, Test_Historial_UltimoSnapshotVacio_DevuelveVacio) {
    FormatterPrometheus formatter;

    const auto conDatos = crearSnapshot(
        100, {{"cpu.usage", 10.0, "porcentaje", 100}});
    const auto vacio = crearSnapshot(200, {});

    EXPECT_EQ(formatter.formatearHistorial({conDatos, vacio}), "");
}

// --- Colisiones de nombres ---

TEST(TestFormatterPrometheus, Test_Colision_SufijoEsHashDe8DigitosHex) {
    FormatterPrometheus formatter;

    const auto tipos = nombresDeTipo(formatter.formatear(crearSnapshot(
        1000,
        {
            {"net.rx", 1.0, "bytes", 1000},
            {"net-rx", 2.0, "bytes", 1000},
        })));

    ASSERT_EQ(tipos.size(), 2U);
    const std::regex patron("^chuspapulse_net_rx_[0-9a-f]{8}$");
    EXPECT_TRUE(std::regex_match(tipos[0], patron)) << tipos[0];
    EXPECT_TRUE(std::regex_match(tipos[1], patron)) << tipos[1];
}

TEST(TestFormatterPrometheus, Test_Colision_TresNombresGeneranTresFamilias) {
    FormatterPrometheus formatter;

    const auto tipos = nombresDeTipo(formatter.formatear(crearSnapshot(
        1000,
        {
            {"a.b", 1.0, "u", 1000},
            {"a-b", 2.0, "u", 1000},
            {"a b", 3.0, "u", 1000},
        })));

    ASSERT_EQ(tipos.size(), 3U);
    const std::set<std::string> distintos(tipos.begin(), tipos.end());
    EXPECT_EQ(distintos.size(), 3U);
    for (const std::string& tipo : tipos) {
        EXPECT_EQ(tipo.rfind("chuspapulse_a_b_", 0), 0U) << tipo;
    }
}

TEST(TestFormatterPrometheus, Test_Colision_ConservaLasEtiquetas) {
    FormatterPrometheus formatter;

    const std::string salida = formatter.formatear(crearSnapshot(
        1000,
        {
            {"net.rx{if=\"a\"}", 1.0, "bytes", 1000},
            {"net-rx{if=\"b\"}", 2.0, "bytes", 1000},
        }));

    EXPECT_EQ(contarPrefijo(salida, "# TYPE "), 2U);
    EXPECT_NE(salida.find("{if=\"a\"} 1\n"), std::string::npos);
    EXPECT_NE(salida.find("{if=\"b\"} 2\n"), std::string::npos);
}

TEST(TestFormatterPrometheus, Test_Colision_SaltoDeLineaEnNombreOriginalSeEscapa) {
    FormatterPrometheus formatter;

    const std::string salida = formatter.formatear(crearSnapshot(
        1000,
        {
            {"a\nb", 1.0, "u", 1000},
            {"a_b", 2.0, "u", 1000},
        }));

    // El salto de línea sale como "\n" literal y no parte la línea HELP.
    EXPECT_NE(salida.find("(nombre original: a\\nb)"), std::string::npos);
    EXPECT_EQ(obtenerLineas(salida).size(), 7U);
}

TEST(TestFormatterPrometheus, Test_NombreRepetido_NoEsColision) {
    FormatterPrometheus formatter;

    const std::string salida = formatter.formatear(crearSnapshot(
        1000,
        {
            {"cpu.usage", 1.0, "porcentaje", 1000},
            {"cpu.usage", 2.0, "porcentaje", 1000},
        }));

    EXPECT_EQ(salida.find("nombre original"), std::string::npos);

    const auto tipos = nombresDeTipo(salida);
    ASSERT_EQ(tipos.size(), 1U);
    EXPECT_EQ(tipos[0], "chuspapulse_cpu_usage");
    EXPECT_NE(salida.find("chuspapulse_cpu_usage 1\n"), std::string::npos);
    EXPECT_NE(salida.find("chuspapulse_cpu_usage 2\n"), std::string::npos);
}
