#include <gtest/gtest.h>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>

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

// --- Validación del puerto (issue #1524) ---

TEST(ParsearPuertoTest, AceptaLimites)
{
    EXPECT_EQ(chuspapulse::config::parsearPuerto("1"), 1);
    EXPECT_EQ(chuspapulse::config::parsearPuerto("8080"), 8080);
    EXPECT_EQ(chuspapulse::config::parsearPuerto("65535"), 65535);
}

TEST(ParsearPuertoTest, RechazaInvalidos)
{
    for (const std::string valor :
         {"", "0", "65536", "-1", "abc", "80a", " 80", "80.5", "+80",
          "99999999999"}) {
        EXPECT_THROW(chuspapulse::config::parsearPuerto(valor),
                     std::invalid_argument)
            << "valor: '" << valor << "'";
    }
}

TEST_F(ConfigCompletoTest, PuertoTomlEnLimitesSeAcepta)
{
    {
        std::ofstream out(archivo_);
        out << "[servidor]\npuerto = 1\n";
    }
    EXPECT_EQ(chuspapulse::config::cargar(archivo_).servidor.puerto, 1);

    {
        std::ofstream out(archivo_);
        out << "[servidor]\npuerto = 65535\n";
    }
    EXPECT_EQ(chuspapulse::config::cargar(archivo_).servidor.puerto, 65535);
}

TEST_F(ConfigCompletoTest, PuertoTomlInvalidoLanzaExcepcion)
{
    for (const std::string valor :
         {"0", "65536", "-5", "\"\"", "\"8080\"", "80.5", "4294975376"}) {
        {
            std::ofstream out(archivo_);
            out << "[servidor]\npuerto = " << valor << "\n";
        }
        EXPECT_THROW(chuspapulse::config::cargar(archivo_),
                     std::invalid_argument)
            << "puerto = " << valor;
    }
}

TEST_F(ConfigCompletoTest, PuertoEnvInvalidoLanzaExcepcion)
{
    setenv("CHUSPAPULSE_SERVIDOR_PUERTO", "70000", 1);
    EXPECT_THROW(chuspapulse::config::cargar("no_existe_1524.toml"),
                 std::invalid_argument);

    setenv("CHUSPAPULSE_SERVIDOR_PUERTO", "9091", 1);
    EXPECT_EQ(chuspapulse::config::cargar("no_existe_1524.toml").servidor.puerto,
              9091);

    unsetenv("CHUSPAPULSE_SERVIDOR_PUERTO");
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
