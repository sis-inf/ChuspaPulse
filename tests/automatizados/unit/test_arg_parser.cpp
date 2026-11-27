#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sys/wait.h>

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

    chuspapulse::cli::ResultadoParseo analizar(
        std::vector<std::string> args,
        chuspapulse::cli::CliOptions& opciones)
    {
        args.insert(args.begin(), "chuspapulse");

        std::vector<char*> argv;
        for (auto& a : args) {
            argv.push_back(a.data());
        }

        return chuspapulse::cli::analizar_argumentos(
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
    EXPECT_EQ(opciones.config_path, "chuspapulse.toml");
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
    EXPECT_EQ(opciones.config_path, "chuspapulse.toml");
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

// --- Validación de --interval (issue #1520) ---

TEST_F(ArgParserTest, IntervalValidoSeAplica)
{
    chuspapulse::cli::CliOptions opciones;

    EXPECT_TRUE(parsear({"--interval", "2500"}, opciones));
    EXPECT_EQ(opciones.monitor.interval_ms, 2500);
}

TEST_F(ArgParserTest, IntervalEnLosLimitesSeAcepta)
{
    chuspapulse::cli::CliOptions minimo;
    EXPECT_TRUE(parsear({"--interval", "101"}, minimo));
    EXPECT_EQ(minimo.monitor.interval_ms, 101);

    chuspapulse::cli::CliOptions maximo;
    EXPECT_TRUE(parsear({"--interval", "3600000"}, maximo));
    EXPECT_EQ(maximo.monitor.interval_ms, 3600000);
}

TEST_F(ArgParserTest, IntervalNoNumericoFallaSinExcepcion)
{
    for (const std::string valor :
         {"abc", "100abc", "1.5", " 500", "+500", "-5", ""}) {
        chuspapulse::cli::CliOptions opciones;

        testing::internal::CaptureStderr();
        bool resultado = true;
        EXPECT_NO_THROW(resultado = parsear({"--interval", valor}, opciones))
            << "valor: '" << valor << "'";
        std::string salida = testing::internal::GetCapturedStderr();

        EXPECT_FALSE(resultado) << "valor: '" << valor << "'";
        EXPECT_NE(salida.find("debe ser un entero positivo en milisegundos"),
                  std::string::npos)
            << "valor: '" << valor << "'";
        EXPECT_EQ(opciones.monitor.interval_ms, 1000)
            << "valor: '" << valor << "'";
    }
}

TEST_F(ArgParserTest, IntervalFueraDeRangoFalla)
{
    for (const std::string valor :
         {"0", "100", "3600001", "99999999999"}) {
        chuspapulse::cli::CliOptions opciones;

        testing::internal::CaptureStderr();
        const bool resultado = parsear({"--interval", valor}, opciones);
        std::string salida = testing::internal::GetCapturedStderr();

        EXPECT_FALSE(resultado) << "valor: " << valor;
        EXPECT_NE(salida.find("permitido: 101 a 3600000 ms"), std::string::npos)
            << "valor: " << valor;
        EXPECT_EQ(opciones.monitor.interval_ms, 1000) << "valor: " << valor;
    }
}

TEST_F(ArgParserTest, IntervalSinValorFalla)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    EXPECT_FALSE(parsear({"--interval"}, opciones));
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_NE(salida.find("falta valor para --interval"), std::string::npos);
}

// --- Codigo de salida de --help (issue #1826) ---

TEST_F(ArgParserTest, HelpMuestraAyudaYNoEsError)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStdout();
    const auto resultado = analizar({"--help"}, opciones);
    std::string salida = testing::internal::GetCapturedStdout();

    EXPECT_EQ(resultado, chuspapulse::cli::ResultadoParseo::AyudaMostrada);
    EXPECT_FALSE(salida.empty());
}

TEST_F(ArgParserTest, ArgumentoInvalidoDevuelveError)
{
    chuspapulse::cli::CliOptions opciones;

    testing::internal::CaptureStderr();
    const auto resultado = analizar({"--opcion-invalida"}, opciones);
    std::string salida = testing::internal::GetCapturedStderr();

    EXPECT_EQ(resultado, chuspapulse::cli::ResultadoParseo::Error);
    EXPECT_NE(salida.find("argumento desconocido"), std::string::npos);
}

// --- Codigo de salida real del ejecutable (issue #1826) ---

namespace {

// Busca el ejecutable chuspapulse junto al binario de prueba (build/bin)
// o, si las pruebas estan en build/tests, en build/bin.
fs::path buscarEjecutable()
{
    const fs::path carpeta =
        fs::read_symlink("/proc/self/exe").parent_path();

    for (const fs::path& candidato :
         {carpeta / "chuspapulse",
          carpeta.parent_path() / "bin" / "chuspapulse"}) {
        if (fs::exists(candidato)) {
            return candidato;
        }
    }
    return {};
}

// Ejecuta el programa con los argumentos dados y devuelve su codigo de
// salida (-1 si no termino normalmente).
int ejecutarCodigoSalida(const fs::path& exe, const std::string& argumentos)
{
    const std::string comando =
        "\"" + exe.string() + "\" " + argumentos + " > /dev/null 2>&1";
    const int estado = std::system(comando.c_str());
    return WIFEXITED(estado) ? WEXITSTATUS(estado) : -1;
}

}  // namespace

TEST_F(ArgParserTest, HelpTerminaConCodigoDeSalida0)
{
    const fs::path exe = buscarEjecutable();
    ASSERT_FALSE(exe.empty())
        << "No se encontro el ejecutable chuspapulse junto al test";

    EXPECT_EQ(ejecutarCodigoSalida(exe, "--help"), 0);
}

TEST_F(ArgParserTest, ArgumentoInvalidoTerminaConCodigoDeSalida1)
{
    const fs::path exe = buscarEjecutable();
    ASSERT_FALSE(exe.empty())
        << "No se encontro el ejecutable chuspapulse junto al test";

    EXPECT_EQ(ejecutarCodigoSalida(exe, "--opcion-invalida"), 1);
}
