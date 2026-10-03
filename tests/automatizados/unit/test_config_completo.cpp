#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

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
    auto cfg = pulso::config::porDefecto();

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

    auto cfg = pulso::config::cargar(archivo_);

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
        pulso::config::cargar(archivo_),
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
        pulso::config::cargar(archivo_)
    );
}

TEST_F(ConfigCompletoTest, NivelLogNoDefaultSeRespeta)
{
    std::ofstream out(archivo_);

    out
        << "nivel_log = \"debug\"\n";

    out.close();

    auto cfg = pulso::config::cargar(archivo_);

    EXPECT_EQ(cfg.nivel_log, "debug");
}
