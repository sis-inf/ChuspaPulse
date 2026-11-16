#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "collectors/bateria/bateria_collector.hpp"
#include "core/types.hpp"

namespace fs = std::filesystem;

using chuspapulse::collectors::ICollector;
using chuspapulse::collectors::bateria::BateriaInfo;
using chuspapulse::collectors::bateria::CollectorBateria;
using chuspapulse::collectors::bateria::getBateriaInfo;

namespace {

// Fixture: crea un directorio temporal que simula /sys/class/power_supply/BAT0
class BateriaFalsaTest : public ::testing::Test {
protected:
    void SetUp() override {
        const std::string nombre_test =
            ::testing::UnitTest::GetInstance()->current_test_info()->name();
        dir_ = fs::temp_directory_path() / ("chuspapulse_bat_" + nombre_test);
        fs::remove_all(dir_);
        fs::create_directories(dir_);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(dir_, ec);
    }

    void escribir(const std::string& archivo, const std::string& contenido) {
        std::ofstream f(dir_ / archivo, std::ios::binary);
        f << contenido;
    }

    std::string ruta() const { return dir_.string(); }

    fs::path dir_;
};

}  // namespace

// ---------------------------------------------------------------------------
// getBateriaInfo: lectura normal
// ---------------------------------------------------------------------------

TEST_F(BateriaFalsaTest, LeePorcentajeYEstadoCharging) {
    escribir("capacity", "85\n");
    escribir("status", "Charging\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_TRUE(info.disponible);
    EXPECT_EQ(info.porcentaje, 85);
    EXPECT_EQ(info.estado, "Charging");
}

TEST_F(BateriaFalsaTest, LeeEstadoDischarging) {
    escribir("capacity", "40\n");
    escribir("status", "Discharging\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_TRUE(info.disponible);
    EXPECT_EQ(info.porcentaje, 40);
    EXPECT_EQ(info.estado, "Discharging");
}

TEST_F(BateriaFalsaTest, LeeEstadoFull) {
    escribir("capacity", "100\n");
    escribir("status", "Full\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_TRUE(info.disponible);
    EXPECT_EQ(info.porcentaje, 100);
    EXPECT_EQ(info.estado, "Full");
}

TEST_F(BateriaFalsaTest, LeeEstadoNotCharging) {
    escribir("capacity", "77\n");
    escribir("status", "Not charging\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_TRUE(info.disponible);
    EXPECT_EQ(info.estado, "Not charging");
}

TEST_F(BateriaFalsaTest, LeeEstadoUnknown) {
    escribir("capacity", "50\n");
    escribir("status", "Unknown\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_TRUE(info.disponible);
    EXPECT_EQ(info.estado, "Unknown");
}

// ---------------------------------------------------------------------------
// getBateriaInfo: rangos limite
// ---------------------------------------------------------------------------

TEST_F(BateriaFalsaTest, AceptaPorcentajeMinimoCero) {
    escribir("capacity", "0\n");
    escribir("status", "Discharging\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_TRUE(info.disponible);
    EXPECT_EQ(info.porcentaje, 0);
}

TEST_F(BateriaFalsaTest, AceptaPorcentajeMaximoCien) {
    escribir("capacity", "100\n");
    escribir("status", "Full\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_TRUE(info.disponible);
    EXPECT_EQ(info.porcentaje, 100);
}

TEST_F(BateriaFalsaTest, PorcentajeLeidoEstaEnRangoValido) {
    for (int valor : {0, 1, 25, 50, 99, 100}) {
        escribir("capacity", std::to_string(valor) + "\n");
        escribir("status", "Discharging\n");

        BateriaInfo info = getBateriaInfo(ruta());

        EXPECT_TRUE(info.disponible) << "valor=" << valor;
        EXPECT_GE(info.porcentaje, 0) << "valor=" << valor;
        EXPECT_LE(info.porcentaje, 100) << "valor=" << valor;
    }
}

TEST_F(BateriaFalsaTest, LeeEstadoSinSaltoDeLineaFinal) {
    escribir("capacity", "60");
    escribir("status", "Charging");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_TRUE(info.disponible);
    EXPECT_EQ(info.porcentaje, 60);
    EXPECT_EQ(info.estado, "Charging");
}

// ---------------------------------------------------------------------------
// getBateriaInfo: errores de lectura (sin lanzar excepcion)
// ---------------------------------------------------------------------------

TEST(BateriaErrorTest, DirectorioInexistenteNoDisponible) {
    BateriaInfo info = getBateriaInfo("/ruta/que/no/existe/chuspapulse_bat");

    EXPECT_FALSE(info.disponible);
    EXPECT_EQ(info.porcentaje, 0);
    EXPECT_TRUE(info.estado.empty());
}

TEST(BateriaErrorTest, DirectorioInexistenteNoLanzaExcepcion) {
    EXPECT_NO_THROW(getBateriaInfo("/ruta/que/no/existe/chuspapulse_bat"));
}

TEST(BateriaErrorTest, RutaVaciaNoLanzaExcepcion) {
    BateriaInfo info;
    EXPECT_NO_THROW(info = getBateriaInfo(""));
    EXPECT_FALSE(info.disponible);
}

TEST_F(BateriaFalsaTest, DirectorioVacioNoDisponible) {
    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_FALSE(info.disponible);
    EXPECT_EQ(info.porcentaje, 0);
    EXPECT_TRUE(info.estado.empty());
}

TEST_F(BateriaFalsaTest, FaltaArchivoStatusNoDisponible) {
    escribir("capacity", "80\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_FALSE(info.disponible);
}

TEST_F(BateriaFalsaTest, FaltaArchivoCapacityNoDisponible) {
    escribir("status", "Charging\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_FALSE(info.disponible);
}

TEST_F(BateriaFalsaTest, CapacityNoNumericoNoDisponible) {
    escribir("capacity", "abc\n");
    escribir("status", "Charging\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_FALSE(info.disponible);
    EXPECT_EQ(info.porcentaje, 0);
    EXPECT_TRUE(info.estado.empty());
}

TEST_F(BateriaFalsaTest, CapacityVacioNoDisponible) {
    escribir("capacity", "");
    escribir("status", "Charging\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_FALSE(info.disponible);
}

TEST_F(BateriaFalsaTest, StatusVacioNoDisponible) {
    escribir("capacity", "80\n");
    escribir("status", "");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_FALSE(info.disponible);
    EXPECT_TRUE(info.estado.empty());
}

TEST_F(BateriaFalsaTest, StatusSoloSaltoDeLineaNoDisponible) {
    escribir("capacity", "80\n");
    escribir("status", "\n");

    BateriaInfo info = getBateriaInfo(ruta());

    EXPECT_FALSE(info.disponible);
}

// ---------------------------------------------------------------------------
// getBateriaInfo: lectura real del sistema (puede o no haber bateria)
// ---------------------------------------------------------------------------

TEST(BateriaSistemaTest, LecturaRealNoLanzaExcepcion) {
    EXPECT_NO_THROW(getBateriaInfo());
}

TEST(BateriaSistemaTest, LecturaRealEsCoherente) {
    BateriaInfo info = getBateriaInfo();

    if (info.disponible) {
        EXPECT_GE(info.porcentaje, 0);
        EXPECT_LE(info.porcentaje, 100);
        EXPECT_FALSE(info.estado.empty());
    } else {
        EXPECT_EQ(info.porcentaje, 0);
        EXPECT_TRUE(info.estado.empty());
    }
}

// ---------------------------------------------------------------------------
// CollectorBateria
// ---------------------------------------------------------------------------

TEST(CollectorBateriaTest, NombreEsBateria) {
    CollectorBateria collector;

    EXPECT_EQ(collector.nombre(), "bateria");
}

TEST(CollectorBateriaTest, FuncionaComoICollector) {
    CollectorBateria collector;
    ICollector& base = collector;

    EXPECT_EQ(base.nombre(), "bateria");
    EXPECT_NO_THROW(base.recolectar());
}

TEST(CollectorBateriaTest, RecolectarNoLanzaExcepcion) {
    CollectorBateria collector;

    EXPECT_NO_THROW(collector.recolectar());
}

TEST(CollectorBateriaTest, RecolectarDevuelveVacioOCantidadEsperada) {
    CollectorBateria collector;

    const auto metricas = collector.recolectar();

    // Sin bateria (servidores/CI): vacio. Con bateria: porcentaje y cargando.
    EXPECT_TRUE(metricas.empty() || metricas.size() == 2u)
        << "tamano inesperado: " << metricas.size();
}

TEST(CollectorBateriaTest, MetricasTienenNombresUnidadesYRangosValidos) {
    CollectorBateria collector;

    const auto metricas = collector.recolectar();

    if (metricas.empty()) {
        GTEST_SKIP() << "Este equipo no tiene bateria; no hay metricas que validar.";
    }

    ASSERT_EQ(metricas.size(), 2u);

    EXPECT_EQ(metricas[0].nombre, "bateria.porcentaje");
    EXPECT_EQ(metricas[0].unidad, "porcentaje");
    EXPECT_GE(metricas[0].valor, 0.0);
    EXPECT_LE(metricas[0].valor, 100.0);

    EXPECT_EQ(metricas[1].nombre, "bateria.cargando");
    EXPECT_EQ(metricas[1].unidad, "booleano");
    EXPECT_TRUE(metricas[1].valor == 0.0 || metricas[1].valor == 1.0);

    EXPECT_GT(metricas[0].timestamp, 0);
    EXPECT_EQ(metricas[0].timestamp, metricas[1].timestamp);
}

TEST(CollectorBateriaTest, DosLecturasConsecutivasSonConsistentes) {
    CollectorBateria collector;

    const auto primera = collector.recolectar();
    const auto segunda = collector.recolectar();

    // La disponibilidad de la bateria no cambia entre dos lecturas seguidas.
    EXPECT_EQ(primera.size(), segunda.size());
}
