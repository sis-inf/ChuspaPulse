#include <gtest/gtest.h>
#include <httplib.h>

#include <array>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "alertas/gestor_alertas.hpp"
#include "config/config.hpp"
#include "http/handler_alerts_history.hpp"

// Definidas en utils/signal_handler.cpp
void setupSignalHandler();
bool consume_reload_request();

// Test de integración para validar el endpoint /config
TEST(IntegrationConfigTest, GetConfigReturns200) {
    // Nota: Asumiendo que el entorno de pruebas levanta el servidor o se prueba de forma local
    httplib::Client cli("localhost", 8080); // O el puerto configurado en las pruebas
    
    auto res = cli.Get("/config");
    
    ASSERT_TRUE(res != nullptr);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(res->get_header_value("Content-Type"), "application/json");
    
    // Validar que la respuesta contenga los campos requeridos (interval_ms, http_port, log_level, etc.)
    EXPECT_NE(res->body.find("interval_ms"), std::string::npos);
    EXPECT_NE(res->body.find("http_port"), std::string::npos);
    EXPECT_NE(res->body.find("log_level"), std::string::npos);
}

namespace {

namespace fs = std::filesystem;
using chuspapulse::config::cargar;
using chuspapulse::config::Config;
using chuspapulse::config::ErrorConfig;

constexpr std::array<const char *, 6> kVariablesEntorno = {
    "CHUSPAPULSE_SERVIDOR_HOST",
    "CHUSPAPULSE_SERVIDOR_PUERTO",
    "CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS",
    "CHUSPAPULSE_STORAGE_RUTA_DB",
    "CHUSPAPULSE_NIVEL_LOG",
    "CHUSPAPULSE_OUTPUT_FORMAT",
};

void definirVariable(const char *nombre, const char *valor) {
#ifdef _WIN32
  _putenv_s(nombre, valor);
#else
  setenv(nombre, valor, 1);
#endif
}

void borrarVariable(const char *nombre) {
#ifdef _WIN32
  _putenv_s(nombre, "");
#else
  unsetenv(nombre);
#endif
}

class IntegrationCargaConfigTest : public ::testing::Test {
protected:
  void SetUp() override {
    limpiarVariables();
    directorio_ = fs::temp_directory_path() /
                  (std::string("chuspapulse_config_") +
                   ::testing::UnitTest::GetInstance()
                       ->current_test_info()
                       ->name());
    fs::create_directories(directorio_);
  }

  void TearDown() override {
    limpiarVariables();
    fs::remove_all(directorio_);
  }

  std::string escribirArchivo(const std::string &contenido) const {
    const fs::path ruta = directorio_ / "config.toml";
    std::ofstream archivo(ruta);
    archivo << contenido;
    return ruta.string();
  }

  std::string rutaInexistente() const {
    return (directorio_ / "no_existe.toml").string();
  }

  fs::path directorio_;

private:
  static void limpiarVariables() {
    for (const char *nombre : kVariablesEntorno) {
      borrarVariable(nombre);
    }
  }
};

const char *const kTomlValido = R"(nivel_log = "debug"
output_format = "csv"

[servidor]
host = "127.0.0.1"
puerto = 9090

[sampler]
intervalo_segundos = 30

[storage]
ruta_db = "desde_toml.db"
)";

} // namespace

TEST_F(IntegrationCargaConfigTest, RutaInexistenteCargaValoresPorDefecto) {
  const Config cfg = cargar(rutaInexistente());

  EXPECT_EQ(cfg.servidor.host, "0.0.0.0");
  EXPECT_EQ(cfg.servidor.puerto, 8080);
  EXPECT_EQ(cfg.sampler.intervalo_segundos, 10);
  EXPECT_EQ(cfg.storage.ruta_db, "chuspapulse.db");
  EXPECT_EQ(cfg.nivel_log, "info");
  EXPECT_EQ(cfg.output_format, "json");
}

TEST_F(IntegrationCargaConfigTest, RutaInexistenteAplicaVariablesDeEntorno) {
  definirVariable("CHUSPAPULSE_STORAGE_RUTA_DB", "desde_env.db");
  definirVariable("CHUSPAPULSE_OUTPUT_FORMAT", "prometheus");

  const Config cfg = cargar(rutaInexistente());

  EXPECT_EQ(cfg.storage.ruta_db, "desde_env.db");
  EXPECT_EQ(cfg.output_format, "prometheus");
  EXPECT_EQ(cfg.servidor.puerto, 8080);
}

TEST_F(IntegrationCargaConfigTest, ArchivoValidoCargaValoresDelToml) {
  const Config cfg = cargar(escribirArchivo(kTomlValido));

  EXPECT_EQ(cfg.servidor.host, "127.0.0.1");
  EXPECT_EQ(cfg.servidor.puerto, 9090);
  EXPECT_EQ(cfg.sampler.intervalo_segundos, 30);
  EXPECT_EQ(cfg.storage.ruta_db, "desde_toml.db");
  EXPECT_EQ(cfg.nivel_log, "debug");
  EXPECT_EQ(cfg.output_format, "csv");
}

TEST_F(IntegrationCargaConfigTest, ClavesAusentesUsanValoresPorDefecto) {
  const Config cfg = cargar(escribirArchivo("[servidor]\npuerto = 9090\n"));

  EXPECT_EQ(cfg.servidor.puerto, 9090);
  EXPECT_EQ(cfg.servidor.host, "0.0.0.0");
  EXPECT_EQ(cfg.sampler.intervalo_segundos, 10);
  EXPECT_EQ(cfg.nivel_log, "info");
}

TEST_F(IntegrationCargaConfigTest, VariablesDeEntornoSobrescribenElToml) {
  definirVariable("CHUSPAPULSE_SERVIDOR_HOST", "10.0.0.1");
  definirVariable("CHUSPAPULSE_SERVIDOR_PUERTO", "7070");
  definirVariable("CHUSPAPULSE_SAMPLER_INTERVALO_SEGUNDOS", "60");
  definirVariable("CHUSPAPULSE_NIVEL_LOG", "warn");

  const Config cfg = cargar(escribirArchivo(kTomlValido));

  EXPECT_EQ(cfg.servidor.host, "10.0.0.1");
  EXPECT_EQ(cfg.servidor.puerto, 7070);
  EXPECT_EQ(cfg.sampler.intervalo_segundos, 60);
  EXPECT_EQ(cfg.nivel_log, "warn");
  EXPECT_EQ(cfg.storage.ruta_db, "desde_toml.db");
  EXPECT_EQ(cfg.output_format, "csv");
}

TEST_F(IntegrationCargaConfigTest, TomlMalFormadoLanzaErrorConfig) {
  const std::string ruta = escribirArchivo("[servidor\npuerto = \n");

  EXPECT_THROW(cargar(ruta), ErrorConfig);
}

TEST_F(IntegrationCargaConfigTest, NivelLogNoPermitidoLanzaInvalidArgument) {
  const std::string ruta = escribirArchivo("nivel_log = \"verbose\"\n");

  EXPECT_THROW(cargar(ruta), std::invalid_argument);
}

TEST_F(IntegrationCargaConfigTest, RecargarArchivoValidoActualizaLaConfiguracion) {
  Config cfg = chuspapulse::config::porDefecto();
  const std::string ruta = escribirArchivo(kTomlValido);
  std::string error;

  EXPECT_TRUE(chuspapulse::config::recargar(ruta, cfg, error));
  EXPECT_TRUE(error.empty());
  EXPECT_EQ(cfg.servidor.puerto, 9090);
  EXPECT_EQ(cfg.nivel_log, "debug");
  EXPECT_EQ(cfg.storage.ruta_db, "desde_toml.db");
}

TEST_F(IntegrationCargaConfigTest, RecargarTomlMalFormadoConservaLaConfiguracion) {
  Config cfg = cargar(escribirArchivo(kTomlValido));
  const Config anterior = cfg;
  const std::string ruta = escribirArchivo("[servidor\npuerto = \n");
  std::string error;

  EXPECT_FALSE(chuspapulse::config::recargar(ruta, cfg, error));
  EXPECT_FALSE(error.empty());
  EXPECT_TRUE(chuspapulse::config::camposModificados(anterior, cfg).empty());
}

TEST_F(IntegrationCargaConfigTest, RecargarValorInvalidoConservaLaConfiguracion) {
  Config cfg = cargar(escribirArchivo(kTomlValido));
  const Config anterior = cfg;
  const std::string ruta = escribirArchivo("[servidor]\npuerto = 70000\n");
  std::string error;

  EXPECT_FALSE(chuspapulse::config::recargar(ruta, cfg, error));
  EXPECT_NE(error.find("puerto"), std::string::npos);
  EXPECT_TRUE(chuspapulse::config::camposModificados(anterior, cfg).empty());
}

TEST_F(IntegrationCargaConfigTest, RecargarArchivoInexistenteConservaLaConfiguracion) {
  Config cfg = cargar(escribirArchivo(kTomlValido));
  const Config anterior = cfg;
  std::string error;

  EXPECT_FALSE(chuspapulse::config::recargar(rutaInexistente(), cfg, error));
  EXPECT_NE(error.find("no existe"), std::string::npos);
  EXPECT_TRUE(chuspapulse::config::camposModificados(anterior, cfg).empty());
}

TEST_F(IntegrationCargaConfigTest, CamposModificadosIndicaLosCambios) {
  const Config antes = chuspapulse::config::porDefecto();
  const Config despues = cargar(escribirArchivo(kTomlValido));

  const std::vector<std::string> esperado = {
      "servidor.host",   "servidor.puerto", "sampler.intervalo_segundos",
      "storage.ruta_db", "nivel_log",       "output_format",
  };

  EXPECT_EQ(chuspapulse::config::camposModificados(antes, despues), esperado);
  EXPECT_TRUE(chuspapulse::config::camposModificados(antes, antes).empty());
}

TEST(SenalRecargaTest, SighupSolicitaRecargaSinTerminarElProceso) {
#ifdef SIGHUP
  setupSignalHandler();
  consume_reload_request();

  ASSERT_EQ(std::raise(SIGHUP), 0);

  EXPECT_TRUE(consume_reload_request());
  EXPECT_FALSE(consume_reload_request());
#else
  GTEST_SKIP() << "La plataforma no tiene SIGHUP";
#endif
}
// Errores de /alerts/history: 400 para entradas invalidas, 200 para validas.
class IntegrationAlertsHistoryErroresTest : public ::testing::Test {
protected:
  httplib::Server servidor_;
  chuspapulse::alertas::GestorAlertas gestor_;
  std::thread hilo_;
  int puerto_ = 0;

  void SetUp() override {
    chuspapulse::http::registrarAlertsHistory(servidor_, gestor_);
    puerto_ = servidor_.bind_to_any_port("127.0.0.1");
    ASSERT_GT(puerto_, 0);
    hilo_ = std::thread([this]() { servidor_.listen_after_bind(); });
    servidor_.wait_until_ready();
  }

  void TearDown() override {
    servidor_.stop();
    if (hilo_.joinable()) {
      hilo_.join();
    }
  }

  httplib::Result get(const std::string &ruta) {
    httplib::Client cliente("127.0.0.1", puerto_);
    return cliente.Get(ruta);
  }

  void esperarError400(const std::string &ruta, const std::string &mensaje) {
    auto res = get(ruta);
    ASSERT_TRUE(res) << ruta;
    EXPECT_EQ(res->status, 400) << ruta;
    EXPECT_EQ(res->get_header_value("Content-Type"), "application/json");
    EXPECT_EQ(res->body, "{\"error\":\"parametro invalido: " + mensaje + "\"}")
        << ruta;
  }
};

TEST_F(IntegrationAlertsHistoryErroresTest, ParametrosValidosDevuelven200) {
  auto res = get("/alerts/history?limite=10&offset=0");

  ASSERT_TRUE(res);
  EXPECT_EQ(res->status, 200);
  EXPECT_EQ(res->body, "[]");
}

TEST_F(IntegrationAlertsHistoryErroresTest, LimiteNoNumericoDevuelve400) {
  esperarError400("/alerts/history?limite=abc",
                  "limite debe ser un entero entre 1 y 1000");
}

TEST_F(IntegrationAlertsHistoryErroresTest, LimiteConCaracteresSobrantesDevuelve400) {
  esperarError400("/alerts/history?limite=10abc",
                  "limite debe ser un entero entre 1 y 1000");
}

TEST_F(IntegrationAlertsHistoryErroresTest, LimiteFueraDeRangoDevuelve400) {
  esperarError400("/alerts/history?limite=0",
                  "limite debe ser un entero entre 1 y 1000");
  esperarError400("/alerts/history?limite=1001",
                  "limite debe ser un entero entre 1 y 1000");
}

TEST_F(IntegrationAlertsHistoryErroresTest, OffsetInvalidoDevuelve400) {
  esperarError400("/alerts/history?offset=-1",
                  "offset debe ser un entero mayor o igual a 0");
  esperarError400("/alerts/history?offset=x",
                  "offset debe ser un entero mayor o igual a 0");
}
