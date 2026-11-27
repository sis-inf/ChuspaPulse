#include <gtest/gtest.h>

#include <cstdint>
#include <exception>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "config/config.hpp"

namespace chuspapulse::config {

TEST(ConfigSamplerTest, DefaultValues) {
    Config config;

    ASSERT_EQ(config.sampler.intervalo_segundos, 10);
    ASSERT_EQ(config.servidor.host,              "0.0.0.0");
    ASSERT_EQ(config.servidor.puerto,            8080);
    ASSERT_EQ(config.storage.ruta_db,            "chuspapulse.db");
    ASSERT_EQ(config.nivel_log,                  "info");
}

TEST(UmbralesConfigTest, DefaultValues) {
    UmbralesConfig umbrales;

    EXPECT_DOUBLE_EQ(umbrales.cpu_max,   80.0);
    EXPECT_DOUBLE_EQ(umbrales.ram_max,   85.0);
    EXPECT_DOUBLE_EQ(umbrales.disco_max, 90.0);
}

TEST(MonitorConfigTest, DefaultValues) {
    MonitorConfig monitor;

    EXPECT_EQ(monitor.interval_ms, 1000);
    EXPECT_TRUE(monitor.cpu);
    EXPECT_TRUE(monitor.ram);
    EXPECT_TRUE(monitor.disk);
}

TEST(ErrorConfigTest, WhatPreservesMessage) {
    ErrorConfig error("error de prueba");

    EXPECT_STREQ(error.what(), "error de prueba");
}

// ---------------------------------------------------------------------------
// Casos borde
// ---------------------------------------------------------------------------

TEST(ErrorConfigTest, EmptyMessageIsPreserved) {
    ErrorConfig error("");

    EXPECT_STREQ(error.what(), "");
}

TEST(ErrorConfigTest, LongMessageIsPreserved) {
    const std::string mensaje(10000, 'x');
    ErrorConfig error(mensaje);

    EXPECT_EQ(std::string(error.what()), mensaje);
}

TEST(ErrorConfigTest, SpecialCharactersArePreserved) {
    const std::string mensaje =
        "Error parseando 'a/b.toml': linea 3\n\tcolumna 7 \"valor\"";
    ErrorConfig error(mensaje);

    EXPECT_EQ(std::string(error.what()), mensaje);
}

TEST(ErrorConfigTest, CopyPreservesMessage) {
    const ErrorConfig original("mensaje original");
    const ErrorConfig copia(original);

    EXPECT_STREQ(copia.what(), "mensaje original");
}

TEST(ErrorConfigTest, IsCatchableAsRuntimeError) {
    EXPECT_THROW(throw ErrorConfig("fallo"), std::runtime_error);
}

TEST(ErrorConfigTest, IsCatchableAsStdException) {
    EXPECT_THROW(throw ErrorConfig("fallo"), std::exception);
}

TEST(ErrorConfigTest, IsNotAnInvalidArgument) {
    // cargar() relanza std::invalid_argument y convierte el resto de errores
    // en ErrorConfig: quien captura debe poder distinguirlos.
    EXPECT_FALSE((std::is_base_of<std::invalid_argument, ErrorConfig>::value));
}

TEST(ErrorConfigTest, ConstructorIsExplicit) {
    EXPECT_FALSE((std::is_convertible<std::string, ErrorConfig>::value));
    EXPECT_TRUE((std::is_constructible<ErrorConfig, const std::string&>::value));
}

TEST(ConfigTest, DefaultOutputFormatIsJson) {
    Config config;

    EXPECT_EQ(config.output_format, "json");
}

TEST(ConfigTest, DefaultThresholdsMatchUmbralesConfig) {
    Config config;

    EXPECT_DOUBLE_EQ(config.umbrales.cpu_max,   80.0);
    EXPECT_DOUBLE_EQ(config.umbrales.ram_max,   85.0);
    EXPECT_DOUBLE_EQ(config.umbrales.disco_max, 90.0);
}

TEST(ConfigTest, DefaultsSatisfyValidationRules) {
    // Los valores por defecto deben cumplir las reglas que cargar() exige a
    // un archivo de configuracion: puerto, intervalo, umbrales, nivel_log y
    // output_format.
    Config config;

    EXPECT_GE(config.servidor.puerto, 1);
    EXPECT_LE(config.servidor.puerto, 65535);
    EXPECT_GT(config.sampler.intervalo_segundos, 0);

    EXPECT_GE(config.umbrales.cpu_max, 0.0);
    EXPECT_LE(config.umbrales.cpu_max, 100.0);
    EXPECT_GE(config.umbrales.ram_max, 0.0);
    EXPECT_LE(config.umbrales.ram_max, 100.0);
    EXPECT_GE(config.umbrales.disco_max, 0.0);
    EXPECT_LE(config.umbrales.disco_max, 100.0);

    EXPECT_TRUE(config.nivel_log == "debug" || config.nivel_log == "info" ||
                config.nivel_log == "warn"  || config.nivel_log == "error");
    EXPECT_TRUE(config.output_format == "json" ||
                config.output_format == "csv" ||
                config.output_format == "prometheus" ||
                config.output_format == "text");
}

TEST(ConfigTest, CopyIsIndependentFromOriginal) {
    Config original;
    Config copia = original;

    copia.servidor.host = "127.0.0.1";
    copia.servidor.puerto = 9090;
    copia.umbrales.cpu_max = 50.0;
    copia.output_format = "csv";

    EXPECT_EQ(original.servidor.host, "0.0.0.0");
    EXPECT_EQ(original.servidor.puerto, 8080);
    EXPECT_DOUBLE_EQ(original.umbrales.cpu_max, 80.0);
    EXPECT_EQ(original.output_format, "json");
}

TEST(ConfigTest, PortBoundariesAreStored) {
    Config config;

    config.servidor.puerto = 1;
    EXPECT_EQ(config.servidor.puerto, 1);

    config.servidor.puerto = 65535;
    EXPECT_EQ(config.servidor.puerto, 65535);
}

TEST(ConfigSamplerTest, IntervalKeepsSixtyFourBitRange) {
    Config config;

    // El intervalo es de 64 bits para no truncar valores grandes del TOML.
    EXPECT_TRUE((std::is_same<decltype(config.sampler.intervalo_segundos),
                              std::int64_t>::value));

    config.sampler.intervalo_segundos =
        std::numeric_limits<std::int64_t>::max();
    EXPECT_EQ(config.sampler.intervalo_segundos,
              std::numeric_limits<std::int64_t>::max());
}

TEST(MonitorConfigTest, FlagsCanBeDisabledIndependently) {
    MonitorConfig monitor;

    monitor.cpu = false;

    EXPECT_FALSE(monitor.cpu);
    EXPECT_TRUE(monitor.ram);
    EXPECT_TRUE(monitor.disk);
}

// ---------------------------------------------------------------------------
// resolverFormatoOnce: output_format se aplica como formato por defecto de --once
// ---------------------------------------------------------------------------

TEST(ResolverFormatoOnceTest, SinFormatExplicitoUsaOutputFormatDeLaConfig) {
    Config config;
    config.output_format = "csv";

    EXPECT_EQ(resolverFormatoOnce(config, false, "json"), "csv");
}

TEST(ResolverFormatoOnceTest, FormatExplicitoTienePrioridadSobreLaConfig) {
    Config config;
    config.output_format = "csv";

    EXPECT_EQ(resolverFormatoOnce(config, true, "prometheus"), "prometheus");
}

TEST(ResolverFormatoOnceTest, FormatExplicitoJsonNoSeSustituyePorLaConfig) {
    // --format json explícito debe respetarse aunque la config diga otra cosa.
    Config config;
    config.output_format = "csv";

    EXPECT_EQ(resolverFormatoOnce(config, true, "json"), "json");
}

TEST(ResolverFormatoOnceTest, ConfigPorDefectoDevuelveJson) {
    Config config;

    EXPECT_EQ(resolverFormatoOnce(config, false, "json"), "json");
}

} // namespace chuspapulse::config
