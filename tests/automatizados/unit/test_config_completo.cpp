#include <gtest/gtest.h>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

#include "cli/arg_parser.h"
#include "config/config.hpp"

namespace fs = std::filesystem;

class ConfigCompletoTest : public ::testing::Test {
protected:
    std::string archivo_;

    void SetUp() override {
        archivo_ =
            std::string(::testing::UnitTest::GetInstance()
                            ->current_test_info()
                            ->name()) +
            ".toml";
    }

    void TearDown() override {
        if (fs::exists(archivo_)) {
            fs::remove(archivo_);
        }
    }

    bool parsear(std::vector<std::string> args,
                 chuspapulse::cli::CliOptions& opciones)
    {
        args.insert(args.begin(), "chuspapulse");

        std::vector<char*> argv;
        for (auto& a : args) {
            argv.push_back(a.data());
        }

        return chuspapulse::cli::parse_arguments(
            static_cast<int>(argv.size()), argv.data(), opciones);
    }

    void crearArchivoConIntervalo(int segundos)
    {
        std::ofstream out(archivo_);
        out << "[sampler]\n"
            << "intervalo_segundos = " << segundos << "\n";
    }
};

TEST_F(ConfigCompletoTest, ValoresPorDefecto)
{
    auto cfg = chuspapulse::config::porDefecto();

    EXPECT_EQ(cfg.servidor.puerto, 8080);
    EXPECT_EQ(cfg.sampler.intervalo_segundos, 10);
}

TEST_F(ConfigCompletoTest, CargaArchivoValido)
{
    std::ofstream out(archivo_);

    out
        << "[servidor]\n"
        << "puerto = 9090\n"
        << "\n"
        << "[sampler]\n"
        << "intervalo_segundos = 5\n";

    out.close();

    auto cfg = chuspapulse::config::cargar(archivo_);

    EXPECT_EQ(cfg.servidor.puerto, 9090);
    EXPECT_EQ(cfg.sampler.intervalo_segundos, 5);
}

TEST_F(ConfigCompletoTest, IntervaloInvalidoLanzaExcepcion)
{
    std::ofstream out(archivo_);

    out
        << "[sampler]\n"
        << "intervalo_segundos = -1\n";

    out.close();

    EXPECT_THROW(
        chuspapulse::config::cargar(archivo_),
        std::invalid_argument
    );
}

TEST_F(ConfigCompletoTest, CampoDesconocidoSeIgnora)
{
    std::ofstream out(archivo_);

    out
        << "campo_inexistente = 123\n";

    out.close();

    EXPECT_NO_THROW(
        chuspapulse::config::cargar(archivo_)
    );
}

TEST_F(ConfigCompletoTest, NivelLogNoDefaultSeRespeta)
{
    std::ofstream out(archivo_);

    out
        << "nivel_log = \"debug\"\n";

    out.close();

    auto cfg = chuspapulse::config::cargar(archivo_);

    EXPECT_EQ(cfg.nivel_log, "debug");
}

TEST_F(ConfigCompletoTest, OutputFormatTextSeAcepta)
{
    std::ofstream out(archivo_);

    out
        << "output_format = \"text\"\n";

    out.close();

    auto cfg = chuspapulse::config::cargar(archivo_);

    EXPECT_EQ(cfg.output_format, "text");
}

TEST_F(ConfigCompletoTest, OutputFormatsSoportadosSeAceptan)
{
    for (const std::string formato : {"json", "csv", "prometheus"}) {
        std::ofstream out(archivo_);

        out
            << "output_format = \"" << formato << "\"\n";

        out.close();

        auto cfg = chuspapulse::config::cargar(archivo_);

        EXPECT_EQ(cfg.output_format, formato);
    }
}

TEST_F(ConfigCompletoTest, OutputFormatInvalidoLanzaExcepcion)
{
    std::ofstream out(archivo_);

    out
        << "output_format = \"xml\"\n";

    out.close();

    try {
        chuspapulse::config::cargar(archivo_);
        FAIL() << "Se esperaba std::invalid_argument";
    } catch (const std::invalid_argument& e) {
        EXPECT_NE(std::string(e.what()).find("output_format: xml"),
                  std::string::npos);
    }
}

TEST_F(ConfigCompletoTest, OutputFormatInvalidoPorEnvSinArchivoLanzaExcepcion)
{
    setenv("CHUSPAPULSE_OUTPUT_FORMAT", "xml", 1);

    EXPECT_THROW(
        chuspapulse::config::cargar("no_existe.toml"),
        std::invalid_argument
    );

    unsetenv("CHUSPAPULSE_OUTPUT_FORMAT");
}

TEST_F(ConfigCompletoTest, SinIntervalCliSeUsaIntervaloPorDefecto)
{
    auto cfg = chuspapulse::config::porDefecto();
    chuspapulse::cli::CliOptions opciones;

    EXPECT_TRUE(parsear({"--once"}, opciones));
    EXPECT_FALSE(opciones.interval_cli);
    EXPECT_EQ(chuspapulse::cli::resolver_intervalo_ms(opciones, cfg), 10000);
}

TEST_F(ConfigCompletoTest, SinIntervalCliSeUsaIntervaloDelArchivo)
{
    crearArchivoConIntervalo(5);
    auto cfg = chuspapulse::config::cargar(archivo_);
    chuspapulse::cli::CliOptions opciones;

    EXPECT_TRUE(parsear({}, opciones));
    EXPECT_EQ(chuspapulse::cli::resolver_intervalo_ms(opciones, cfg), 5000);
}

TEST_F(ConfigCompletoTest, IntervalCliPrevaleceSobreArchivo)
{
    crearArchivoConIntervalo(5);
    auto cfg = chuspapulse::config::cargar(archivo_);
    chuspapulse::cli::CliOptions opciones;

    EXPECT_TRUE(parsear({"--interval", "250"}, opciones));
    EXPECT_TRUE(opciones.interval_cli);
    EXPECT_EQ(chuspapulse::cli::resolver_intervalo_ms(opciones, cfg), 250);
}

TEST_F(ConfigCompletoTest, IntervalCliPrevaleceSobreVariableDeEntorno)
{
    setenv("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS", "7", 1);
    auto cfg = chuspapulse::config::cargar("no_existe.toml");
    unsetenv("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS");

    chuspapulse::cli::CliOptions sin_cli;
    EXPECT_TRUE(parsear({}, sin_cli));
    EXPECT_EQ(chuspapulse::cli::resolver_intervalo_ms(sin_cli, cfg), 7000);

    chuspapulse::cli::CliOptions con_cli;
    EXPECT_TRUE(parsear({"--interval", "500"}, con_cli));
    EXPECT_EQ(chuspapulse::cli::resolver_intervalo_ms(con_cli, cfg), 500);
}

TEST_F(ConfigCompletoTest, IntervalCliFueraDeRangoSeRechaza)
{
    auto cfg = chuspapulse::config::porDefecto();
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--interval", "100"}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("mayor a 100ms"), std::string::npos);
    EXPECT_FALSE(opciones.interval_cli);
    EXPECT_EQ(chuspapulse::cli::resolver_intervalo_ms(opciones, cfg), 10000);
}
