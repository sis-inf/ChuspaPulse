#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "cli/arg_parser.h"

namespace fs = std::filesystem;

class ArgParserTest : public ::testing::Test {
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
            fs::permissions(archivo_, fs::perms::owner_all,
                            fs::perm_options::add);
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

    void crearArchivo()
    {
        std::ofstream out(archivo_);
        out << "nivel_log = \"info\"\n";
    }
};

TEST_F(ArgParserTest, SinConfigConservaRutaPredeterminada)
{
    chuspapulse::cli::CliOptions opciones;

    EXPECT_TRUE(parsear({"--once"}, opciones));
    EXPECT_EQ(opciones.config_path, "pulso.toml");
}

TEST_F(ArgParserTest, ConfigSinValorFalla)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--config"}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("falta ruta para --config"), std::string::npos);
    EXPECT_NE(salida.find("Uso: --config <ruta>"), std::string::npos);
}

TEST_F(ArgParserTest, ConfigSeguidoDeOtraOpcionFalla)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--config", "--once"}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("falta ruta para --config"), std::string::npos);
}

TEST_F(ArgParserTest, ConfigConRutaInexistenteFalla)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--config", "no_existe.toml"}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("no existe: no_existe.toml"), std::string::npos);
    EXPECT_EQ(opciones.config_path, "pulso.toml");
}

TEST_F(ArgParserTest, ConfigConArchivoNoLegibleFalla)
{
    crearArchivo();
    fs::permissions(archivo_, fs::perms::none);

    if (std::ifstream(archivo_).is_open()) {
        GTEST_SKIP() << "El usuario actual puede leer archivos sin permisos";
    }

    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--config", archivo_}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("no se puede leer"), std::string::npos);
}

TEST_F(ArgParserTest, ConfigConRutaValidaSeGuarda)
{
    crearArchivo();

    chuspapulse::cli::CliOptions opciones;

    EXPECT_TRUE(parsear({"--config", archivo_}, opciones));
    EXPECT_EQ(opciones.config_path, archivo_);
}

TEST_F(ArgParserTest, MetricsConNombreValidoSeAplica)
{
    chuspapulse::cli::CliOptions opciones;

    EXPECT_TRUE(parsear({"--metrics", "cpu"}, opciones));
    EXPECT_TRUE(opciones.monitor.cpu);
    EXPECT_FALSE(opciones.monitor.ram);
    EXPECT_FALSE(opciones.monitor.disk);
}

TEST_F(ArgParserTest, MetricsConVariosNombresValidosSeAplican)
{
    chuspapulse::cli::CliOptions opciones;

    EXPECT_TRUE(parsear({"--metrics", "cpu,ram,disk"}, opciones));
    EXPECT_TRUE(opciones.monitor.cpu);
    EXPECT_TRUE(opciones.monitor.ram);
    EXPECT_TRUE(opciones.monitor.disk);
}

TEST_F(ArgParserTest, MetricsConNombreDesconocidoFalla)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--metrics", "gpu"}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("metricas desconocidas: gpu"), std::string::npos);
    EXPECT_NE(salida.find("Metricas validas: cpu, ram, disk"),
              std::string::npos);
}

TEST_F(ArgParserTest, MetricsMezcladasReportaDesconocidasSinModificarConfig)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--metrics", "cpu,gpu,red"}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("metricas desconocidas: gpu, red"),
              std::string::npos);
    EXPECT_TRUE(opciones.monitor.cpu);
    EXPECT_TRUE(opciones.monitor.ram);
    EXPECT_TRUE(opciones.monitor.disk);
}

TEST_F(ArgParserTest, MetricsConListaVaciaFalla)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--metrics", ""}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("lista de --metrics esta vacia"), std::string::npos);
}

TEST_F(ArgParserTest, MetricsConElementoVacioFalla)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--metrics", "cpu,"}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("metricas desconocidas: \"\""), std::string::npos);
}
