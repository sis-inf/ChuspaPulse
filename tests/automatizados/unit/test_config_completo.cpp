#include <gtest/gtest.h>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <stdexcept>
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

TEST_F(ConfigCompletoTest, CargaTodasLasSeccionesDocumentadas)
{
    std::ofstream out(archivo_);
    out << "[general]\nnivel_log = \"warn\"\noutput_format = \"csv\"\n"
        << "[http]\nhost = \"127.0.0.1\"\npuerto = 9011\n"
        << "[sampler]\nintervalo_segundos = 25\n"
        << "[storage]\nruta_db = \"pruebas.db\"\n"
        << "[alertas]\ncpu_max = 41.5\nram_max = 51\ndisco_max = 61.5\n";
    out.close();

    const auto cfg = chuspapulse::config::cargar(archivo_);
    EXPECT_EQ(cfg.nivel_log, "warn");
    EXPECT_EQ(cfg.output_format, "csv");
    EXPECT_EQ(cfg.servidor.host, "127.0.0.1");
    EXPECT_EQ(cfg.servidor.puerto, 9011);
    EXPECT_EQ(cfg.sampler.intervalo_segundos, 25);
    EXPECT_EQ(cfg.storage.ruta_db, "pruebas.db");
    EXPECT_DOUBLE_EQ(cfg.umbrales.cpu_max, 41.5);
    EXPECT_DOUBLE_EQ(cfg.umbrales.ram_max, 51);
    EXPECT_DOUBLE_EQ(cfg.umbrales.disco_max, 61.5);
}

TEST_F(ConfigCompletoTest, UsaValoresAnterioresCuandoFaltanClavesNuevas)
{
    std::ofstream out(archivo_);
    out << "nivel_log = \"debug\"\noutput_format = \"text\"\n"
        << "[servidor]\nhost = \"127.0.0.2\"\npuerto = 9012\n"
        << "[umbrales]\ncpu_max = 42.5\nram_max = 52\ndisco_max = 62.5\n";
    out.close();

    const auto cfg = chuspapulse::config::cargar(archivo_);
    EXPECT_EQ(cfg.nivel_log, "debug");
    EXPECT_EQ(cfg.output_format, "text");
    EXPECT_EQ(cfg.servidor.host, "127.0.0.2");
    EXPECT_EQ(cfg.servidor.puerto, 9012);
    EXPECT_DOUBLE_EQ(cfg.umbrales.cpu_max, 42.5);
    EXPECT_DOUBLE_EQ(cfg.umbrales.ram_max, 52);
    EXPECT_DOUBLE_EQ(cfg.umbrales.disco_max, 62.5);
}

TEST_F(ConfigCompletoTest, FormatoNuevoPrioritarioPorClave)
{
    std::ofstream out(archivo_);
    out << "nivel_log = \"debug\"\n"
        << "[servidor]\nhost = \"127.0.0.3\"\npuerto = \"invalido\"\n"
        << "[umbrales]\ncpu_max = 43\nram_max = 53\n"
        << "[general]\nnivel_log = \"error\"\n"
        << "[http]\npuerto = 9013\n"
        << "[alertas]\nram_max = 63\n";
    out.close();

    const auto cfg = chuspapulse::config::cargar(archivo_);
    EXPECT_EQ(cfg.nivel_log, "error");
    EXPECT_EQ(cfg.servidor.host, "127.0.0.3");
    EXPECT_EQ(cfg.servidor.puerto, 9013);
    EXPECT_DOUBLE_EQ(cfg.umbrales.cpu_max, 43);
    EXPECT_DOUBLE_EQ(cfg.umbrales.ram_max, 63);
}

TEST_F(ConfigCompletoTest, PuertoNuevoInvalidoSeRechaza)
{
    std::ofstream out(archivo_);
    out << "[servidor]\npuerto = 9014\n[http]\npuerto = \"9015\"\n";
    out.close();

    EXPECT_THROW(chuspapulse::config::cargar(archivo_), chuspapulse::config::ErrorConfig);
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
        chuspapulse::config::ErrorConfig
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
                     chuspapulse::config::ErrorConfig
            << "puerto = " << valor;
    }
}

TEST_F(ConfigCompletoTest, PuertoEnvInvalidoLanzaExcepcion)
{
    setenv("CHUSPAPULSE_SERVIDOR_PUERTO", "70000", 1);
    EXPECT_THROW(chuspapulse::config::cargar("no_existe_1524.toml"),
                 chuspapulse::config::ErrorConfig

    setenv("CHUSPAPULSE_SERVIDOR_PUERTO", "9091", 1);
    EXPECT_EQ(chuspapulse::config::cargar("no_existe_1524.toml").servidor.puerto,
              9091);

    unsetenv("CHUSPAPULSE_SERVIDOR_PUERTO");
}

TEST_F(ConfigCompletoTest, IntervaloEnvInvalidoLanzaErrorConfig)
{
    for (const char* valor : {"30s", "abc", "0", "-5"}) {
        setenv("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS", valor, 1);
        EXPECT_THROW(chuspapulse::config::cargar("no_existe_1823.toml"),
                     chuspapulse::config::ErrorConfig)
            << "env = " << valor;
    }

    unsetenv("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS");
}

TEST_F(ConfigCompletoTest, IntervaloEnvValidoSeAcepta)
{
    setenv("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS", "15", 1);
    EXPECT_EQ(chuspapulse::config::cargar("no_existe_1823.toml")
                  .sampler.intervalo_segundos,
              15);

    unsetenv("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS");
}

TEST_F(ConfigCompletoTest, IntervaloEnvInvalidoConArchivoValidoLanzaErrorConfig)
{
    {
        std::ofstream out(archivo_);
        out << "[sampler]\nintervalo_segundos = 5\n";
    }

    setenv("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS", "abc", 1);
    EXPECT_THROW(chuspapulse::config::cargar(archivo_),
                 chuspapulse::config::ErrorConfig);

    unsetenv("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS");
}

TEST_F(ConfigCompletoTest, ArchivoTomlInvalidoLanzaErrorConfig)
{
    {
        std::ofstream out(archivo_);
        out << "[servidor\npuerto = no_valido ===\n";
    }

    EXPECT_THROW(chuspapulse::config::cargar(archivo_),
                 chuspapulse::config::ErrorConfig);
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
        FAIL() << "Se esperaba ErrorConfig";
    } catch (const chuspapulse::config::ErrorConfig& e) {
        EXPECT_NE(std::string(e.what()).find("output_format: xml"),
                  std::string::npos);
    }
}

TEST_F(ConfigCompletoTest, OutputFormatInvalidoPorEnvSinArchivoLanzaExcepcion)
{
    setenv("CHUSPAPULSE_OUTPUT_FORMAT", "xml", 1);

    EXPECT_THROW(
        chuspapulse::config::cargar("no_existe.toml"),
        chuspapulse::config::ErrorConfig
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

    EXPECT_NE(salida.find("fuera de rango"), std::string::npos);
    EXPECT_FALSE(opciones.interval_cli);
    EXPECT_EQ(chuspapulse::cli::resolver_intervalo_ms(opciones, cfg), 10000);
}
// --- Casos borde: umbrales/métricas y condiciones de error (issue #1693) ---

TEST_F(ConfigCompletoTest, UmbralesEnLimitesSeAceptan)
{
    std::ofstream out(archivo_);
    out << "[alertas]\n"
        << "cpu_max = 0.0\n"
        << "ram_max = 0.0\n"
        << "disco_max = 100.0\n";
    out.close();

    const auto cfg = chuspapulse::config::cargar(archivo_);
    EXPECT_DOUBLE_EQ(cfg.umbrales.cpu_max, 0.0);
    EXPECT_DOUBLE_EQ(cfg.umbrales.ram_max, 0.0);
    EXPECT_DOUBLE_EQ(cfg.umbrales.disco_max, 100.0);
}

TEST_F(ConfigCompletoTest, UmbralesFueraDeRangoLanzanExcepcion)
{
    for (const std::string campo : {"cpu_max", "ram_max", "disco_max"}) {
        for (const std::string valor : {"-1.0", "100.1", "150.0"}) {
            {
                std::ofstream out(archivo_);
                out << "[alertas]\n"
                    << campo << " = " << valor << "\n";
                out.close();
            }
            EXPECT_THROW(chuspapulse::config::cargar(archivo_),
                         chuspapulse::config::ErrorConfig
                << campo << " = " << valor;
        }
    }
}

TEST_F(ConfigCompletoTest, IntervaloCeroEnTomlLanzaExcepcion)
{
    crearArchivoConIntervalo(0);
    EXPECT_THROW(chuspapulse::config::cargar(archivo_),
                 chuspapulse::config::ErrorConfig
}

TEST_F(ConfigCompletoTest, ArchivoTomlInvalidoLanzaExcepcion)
{
    std::ofstream out(archivo_);
    out << "[servidor\npuerto = no_valido ===\n";
    out.close();

    EXPECT_THROW(chuspapulse::config::cargar(archivo_),
                 std::exception);
}