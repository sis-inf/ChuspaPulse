// test_ram_usage.cpp (unit)
// Pruebas unitarias del recolector de memoria RAM: getRamUsage() y
// CollectorMemory. Los casos de getRamUsage() usan archivos mock con formato
// /proc/meminfo (ruta inyectada), por lo que son reproducibles. Los casos que
// leen el sistema real se omiten (GTEST_SKIP) si /proc/meminfo no existe.

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>

#include "collectors/memory/ram_usage.hpp"

using chuspapulse::collectors::memory::CollectorMemory;
using chuspapulse::collectors::memory::getRamUsage;
using chuspapulse::collectors::memory::RamInfo;

namespace {

constexpr uint64_t kKiB = 1024;

bool escribirArchivo(const std::string& ruta, const std::string& contenido) {
  std::ofstream f(ruta);
  if (!f.is_open()) {
    return false;
  }
  f << contenido;
  return f.good();
}

// Contenido mínimo válido con el formato de /proc/meminfo (valores en kB).
std::string meminfo(long long total_kb, long long available_kb) {
  return "MemTotal:       " + std::to_string(total_kb) +
         " kB\nMemFree:        1000 kB\nMemAvailable:   " +
         std::to_string(available_kb) + " kB\n";
}

bool procMeminfoDisponible() {
  std::ifstream f("/proc/meminfo");
  return f.is_open();
}

class RamUsageUnitTest : public ::testing::Test {
 protected:
  std::string ruta_;

  void SetUp() override {
    const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
    ruta_ = ::testing::TempDir() + "/meminfo_unit_" + info->name() + ".txt";
  }

  void TearDown() override { std::remove(ruta_.c_str()); }
};

}  // namespace

// ---------------------------------------------------------------------------
// Recolección normal y rangos esperados
// ---------------------------------------------------------------------------

TEST_F(RamUsageUnitTest, ConvierteKbABytes) {
  ASSERT_TRUE(escribirArchivo(ruta_, meminfo(16'384'000, 8'192'000)));

  RamInfo info = getRamUsage(ruta_);

  EXPECT_EQ(info.total, 16'384'000ULL * kKiB);
  EXPECT_EQ(info.available, 8'192'000ULL * kKiB);
}

TEST_F(RamUsageUnitTest, UsadoEsTotalMenosDisponible) {
  ASSERT_TRUE(escribirArchivo(ruta_, meminfo(8'123'456, 4'194'304)));

  RamInfo info = getRamUsage(ruta_);

  EXPECT_EQ(info.used, info.total - info.available);
  EXPECT_EQ(info.used, (8'123'456ULL - 4'194'304ULL) * kKiB);
}

TEST_F(RamUsageUnitTest, CumpleInvariantesDeRango) {
  ASSERT_TRUE(escribirArchivo(ruta_, meminfo(8'000'000, 3'000'000)));

  RamInfo info = getRamUsage(ruta_);

  EXPECT_GT(info.total, 0u);
  EXPECT_LE(info.used, info.total);
  EXPECT_LE(info.available, info.total);
  EXPECT_EQ(info.used + info.available, info.total);
}

TEST_F(RamUsageUnitTest, TodaLaMemoriaDisponibleDejaUsadoEnCero) {
  ASSERT_TRUE(escribirArchivo(ruta_, meminfo(4'000'000, 4'000'000)));

  RamInfo info = getRamUsage(ruta_);

  EXPECT_EQ(info.used, 0u);
  EXPECT_EQ(info.available, info.total);
}

TEST_F(RamUsageUnitTest, CasiTodaLaMemoriaOcupada) {
  ASSERT_TRUE(escribirArchivo(ruta_, meminfo(8'000'000, 1)));

  RamInfo info = getRamUsage(ruta_);

  EXPECT_EQ(info.available, 1ULL * kKiB);
  EXPECT_EQ(info.used, (8'000'000ULL - 1ULL) * kKiB);
}

TEST_F(RamUsageUnitTest, SoportaValoresMayoresA32Bits) {
  // 4 TiB expresados en kB; en bytes supera 2^32.
  const long long total_kb = 4'294'967'296LL;
  const long long available_kb = 1'073'741'824LL;
  ASSERT_TRUE(escribirArchivo(ruta_, meminfo(total_kb, available_kb)));

  RamInfo info = getRamUsage(ruta_);

  EXPECT_EQ(info.total, static_cast<uint64_t>(total_kb) * kKiB);
  EXPECT_EQ(info.available, static_cast<uint64_t>(available_kb) * kKiB);
  EXPECT_EQ(info.used, info.total - info.available);
}

TEST_F(RamUsageUnitTest, LasClavesPuedenVenirEnCualquierOrden) {
  const std::string contenido =
      "MemAvailable:   2000000 kB\n"
      "Buffers:         512000 kB\n"
      "MemTotal:       6000000 kB\n";
  ASSERT_TRUE(escribirArchivo(ruta_, contenido));

  RamInfo info = getRamUsage(ruta_);

  EXPECT_EQ(info.total, 6'000'000ULL * kKiB);
  EXPECT_EQ(info.available, 2'000'000ULL * kKiB);
  EXPECT_EQ(info.used, 4'000'000ULL * kKiB);
}

TEST_F(RamUsageUnitTest, IgnoraLineasDesconocidasYVacias) {
  const std::string contenido =
      "\n"
      "Linea sin formato\n"
      "MemTotal:       8000000 kB\n"
      "\n"
      "Cached:         2048000 kB\n"
      "SwapTotal:      2097152 kB\n"
      "MemAvailable:   5000000 kB\n"
      "HugePages_Total:       0\n";
  ASSERT_TRUE(escribirArchivo(ruta_, contenido));

  RamInfo info = getRamUsage(ruta_);

  EXPECT_EQ(info.total, 8'000'000ULL * kKiB);
  EXPECT_EQ(info.available, 5'000'000ULL * kKiB);
}

TEST_F(RamUsageUnitTest, LecturasRepetidasDanElMismoResultado) {
  ASSERT_TRUE(escribirArchivo(ruta_, meminfo(8'000'000, 3'000'000)));

  RamInfo primera = getRamUsage(ruta_);
  RamInfo segunda = getRamUsage(ruta_);

  EXPECT_EQ(primera.total, segunda.total);
  EXPECT_EQ(primera.used, segunda.used);
  EXPECT_EQ(primera.available, segunda.available);
}

// ---------------------------------------------------------------------------
// Errores de lectura
// ---------------------------------------------------------------------------

TEST_F(RamUsageUnitTest, RutaInexistenteLanzaRuntimeErrorConLaRuta) {
  const std::string inexistente =
      ::testing::TempDir() + "/no_existe_meminfo.txt";

  try {
    getRamUsage(inexistente);
    FAIL() << "Se esperaba std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_NE(std::string(e.what()).find(inexistente), std::string::npos);
  }
}

TEST_F(RamUsageUnitTest, RutaQueEsDirectorioLanzaRuntimeError) {
  EXPECT_THROW(getRamUsage(::testing::TempDir()), std::runtime_error);
}

TEST_F(RamUsageUnitTest, ArchivoVacioLanzaRuntimeError) {
  ASSERT_TRUE(escribirArchivo(ruta_, ""));

  EXPECT_THROW(getRamUsage(ruta_), std::runtime_error);
}

TEST_F(RamUsageUnitTest, SinMemTotalLanzaRuntimeError) {
  ASSERT_TRUE(escribirArchivo(
      ruta_, "MemFree:  1000 kB\nMemAvailable:  3000000 kB\n"));

  EXPECT_THROW(getRamUsage(ruta_), std::runtime_error);
}

TEST_F(RamUsageUnitTest, SinMemAvailableLanzaRuntimeError) {
  ASSERT_TRUE(
      escribirArchivo(ruta_, "MemTotal:  8000000 kB\nMemFree:  1000 kB\n"));

  EXPECT_THROW(getRamUsage(ruta_), std::runtime_error);
}

TEST_F(RamUsageUnitTest, MemTotalEnCeroLanzaRuntimeError) {
  ASSERT_TRUE(escribirArchivo(ruta_, meminfo(0, 3'000'000)));

  EXPECT_THROW(getRamUsage(ruta_), std::runtime_error);
}

TEST_F(RamUsageUnitTest, MemAvailableEnCeroSeConsideraInvalido) {
  // Comportamiento actual: un valor 0 se trata como clave ausente.
  ASSERT_TRUE(escribirArchivo(ruta_, meminfo(8'000'000, 0)));

  EXPECT_THROW(getRamUsage(ruta_), std::runtime_error);
}

TEST_F(RamUsageUnitTest, ValorNoNumericoLanzaRuntimeError) {
  ASSERT_TRUE(escribirArchivo(
      ruta_, "MemTotal:  abc kB\nMemAvailable:  3000000 kB\n"));

  EXPECT_THROW(getRamUsage(ruta_), std::runtime_error);
}

// ---------------------------------------------------------------------------
// Sistema real (se omite si no hay /proc/meminfo)
// ---------------------------------------------------------------------------

TEST(RamUsageSistemaRealTest, LecturaRealCumpleRangos) {
  if (!procMeminfoDisponible()) {
    GTEST_SKIP() << "/proc/meminfo no disponible en este sistema";
  }

  RamInfo info = getRamUsage();

  EXPECT_GT(info.total, 0u);
  EXPECT_LE(info.used, info.total);
  EXPECT_LE(info.available, info.total);
  EXPECT_EQ(info.used + info.available, info.total);
}

// ---------------------------------------------------------------------------
// CollectorMemory
// ---------------------------------------------------------------------------

TEST(CollectorMemoryTest, NombreEsMemory) {
  CollectorMemory collector;

  EXPECT_EQ(collector.nombre(), "memory");
}

TEST(CollectorMemoryTest, RecolectarDevuelveMetricaMemoryUsed) {
  if (!procMeminfoDisponible()) {
    GTEST_SKIP() << "/proc/meminfo no disponible en este sistema";
  }

  CollectorMemory collector;
  auto metricas = collector.recolectar();

  ASSERT_EQ(metricas.size(), 1u);
  EXPECT_EQ(metricas[0].nombre, "memory.used");
  EXPECT_EQ(metricas[0].unidad, "bytes");
  EXPECT_GE(metricas[0].valor, 0.0);
  EXPECT_GT(metricas[0].timestamp, 0);
}

TEST(CollectorMemoryTest, ValorRecolectadoNoSuperaLaMemoriaTotal) {
  if (!procMeminfoDisponible()) {
    GTEST_SKIP() << "/proc/meminfo no disponible en este sistema";
  }

  CollectorMemory collector;
  auto metricas = collector.recolectar();
  RamInfo info = getRamUsage();

  ASSERT_EQ(metricas.size(), 1u);
  EXPECT_LE(metricas[0].valor, static_cast<double>(info.total));
}