#include <gtest/gtest.h>
#include "config/config.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <system_error>

namespace chuspapulse::config {

// Pruebas originales: valores predeterminados.
TEST(ConfigSamplerTest, DefaultValues) {
    Config config;

    ASSERT_EQ(config.sampler.intervalo_segundos, 10);
    ASSERT_EQ(config.servidor.host, "0.0.0.0");
    ASSERT_EQ(config.servidor.puerto, 8080);
    ASSERT_EQ(config.storage.ruta_db, "chuspapulse.db");
    ASSERT_EQ(config.nivel_log, "info");
}

TEST(UmbralesConfigTest, DefaultValues) {
    UmbralesConfig umbrales;

    EXPECT_DOUBLE_EQ(umbrales.cpu_max, 80.0);
    EXPECT_DOUBLE_EQ(umbrales.ram_max, 85.0);
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

namespace {

// Cada prueba utiliza su propio archivo temporal.
class ConfigEdgeTest : public ::testing::Test {
protected:
    std::filesystem::path directorio;
    std::filesystem::path archivo;

    void SetUp() override {
        const auto identificador =
            std::chrono::steady_clock::now()
                .time_since_epoch()
                .count();

        directorio = std::filesystem::temp_directory_path() /
            ("chuspapulse_config_" + std::to_string(identificador));

        ASSERT_TRUE(std::filesystem::create_directory(directorio));
        archivo = directorio / "config.toml";
    }

    void TearDown() override {
        std::error_code error;
        std::filesystem::remove_all(directorio, error);
    }

    void escribir(const std::string& contenido) {
        std::ofstream salida(archivo);
        ASSERT_TRUE(salida.is_open());

        salida << contenido;
        salida.close();

        ASSERT_FALSE(salida.fail());
    }
};

// Los porcentajes 0 y 100 deben ser aceptados.
TEST_F(ConfigEdgeTest, AcceptsThresholdBoundaries) {
    for (const auto* valor : {"0.0", "100.0"}) {
        SCOPED_TRACE(valor);

        ASSERT_NO_FATAL_FAILURE(escribir(
            std::string("[umbrales]\ncpu_max = ") + valor +
            "\nram_max = " + valor +
            "\ndisco_max = " + valor + "\n"));

        Config config;
        ASSERT_NO_THROW(config = cargar(archivo.string()));

        const double esperado = std::stod(valor);

        EXPECT_DOUBLE_EQ(config.umbrales.cpu_max, esperado);
        EXPECT_DOUBLE_EQ(config.umbrales.ram_max, esperado);
        EXPECT_DOUBLE_EQ(config.umbrales.disco_max, esperado);
    }
}

// Rechaza porcentajes fuera de rango, NaN e infinitos.
TEST_F(ConfigEdgeTest, RejectsInvalidThresholds) {
    for (const auto* campo : {"cpu_max", "ram_max", "disco_max"}) {
        for (const auto* valor :
             {"-0.1", "100.1", "nan", "inf", "-inf"}) {
            SCOPED_TRACE(std::string(campo) + " = " + valor);

            ASSERT_NO_FATAL_FAILURE(escribir(
                std::string("[umbrales]\n") +
                campo + " = " + valor + "\n"));

            EXPECT_THROW(
                cargar(archivo.string()),
                std::invalid_argument);
        }
    }
}

// El intervalo de muestreo debe ser mayor que cero.
TEST_F(ConfigEdgeTest, RejectsNonPositiveSamplerInterval) {
    for (const auto* valor : {"0", "-1"}) {
        SCOPED_TRACE(valor);

        ASSERT_NO_FATAL_FAILURE(escribir(
            std::string("[sampler]\nintervalo_segundos = ") +
            valor + "\n"));

        EXPECT_THROW(
            cargar(archivo.string()),
            std::invalid_argument);
    }
}

// Un archivo TOML mal escrito debe generar ErrorConfig.
TEST_F(ConfigEdgeTest, RejectsMalformedToml) {
    ASSERT_NO_FATAL_FAILURE(escribir(
        "[umbrales\ncpu_max = 80.0\n"));

    EXPECT_THROW(cargar(archivo.string()), ErrorConfig);
}

// Una recarga fallida debe conservar la configuración anterior.
TEST_F(ConfigEdgeTest, FailedReloadPreservesConfiguration) {
    Config config;
    config.servidor.puerto = 9090;
    config.umbrales.cpu_max = 42.0;

    const Config original = config;
    std::string error;

    ASSERT_NO_FATAL_FAILURE(escribir(
        "[umbrales]\ncpu_max = 100.1\n"));

    EXPECT_FALSE(recargar(archivo.string(), config, error));
    EXPECT_FALSE(error.empty());
    EXPECT_TRUE(camposModificados(original, config).empty());
}

} // namespace

} // namespace chuspapulse::config
