#include <gtest/gtest.h>
#include <httplib.h>

#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "config/config.hpp"

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