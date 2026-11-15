// test_ram_usage.cpp
// Pruebas unitarias para getRamUsage() usando un archivo mock de /proc/meminfo.
// Los tests son completamente reproducibles: no se lee ningún archivo real del sistema.

#include <gtest/gtest.h>

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>

#include "collectors/memory/ram_usage.hpp"

using chuspapulse::collectors::memory::getRamUsage;
using chuspapulse::collectors::memory::RamInfo;

namespace {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Escribe un archivo con formato /proc/meminfo en `path` con los valores dados
// (expresados en kB, igual que el formato real del kernel).
void writeMockMeminfo(const std::string& path,
                      long long total_kb,
                      long long available_kb,
                      long long free_kb) {
  std::ofstream f(path);
  ASSERT_TRUE(f.is_open()) << "No se pudo crear el archivo mock en: " << path;
  f << "MemTotal:     " << total_kb     << " kB\n";
  f << "MemFree:      " << free_kb      << " kB\n";
  f << "MemAvailable: " << available_kb << " kB\n";
}

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------

class RamUsageTest : public ::testing::Test {
 protected:
  // Ruta del archivo temporal; única por ejecución gracias a ::testing::TempDir().
  std::string mock_path_;

  void SetUp() override {
    mock_path_ = std::string(::testing::TempDir()) + "/mock_meminfo.txt";
  }

  void TearDown() override {
    std::remove(mock_path_.c_str());
  }
};

} // namespace

// ---------------------------------------------------------------------------
// Test 1: los valores devueltos en bytes son correctos para entradas conocidas en kB
// ---------------------------------------------------------------------------
TEST_F(RamUsageTest, RetornaBytesCorrectedParaEntradaConocida) {
  // Arrange: escribir valores conocidos (en kB)
  const long long total_kb     = 8'000'000; // 8 GB
  const long long available_kb = 3'000'000; // 3 GB
  const long long free_kb      = 1'500'000; // 1.5 GB (se parsea pero no se usa en el cálculo)

  writeMockMeminfo(mock_path_, total_kb, available_kb, free_kb);

  // Act
  RamInfo info = getRamUsage(mock_path_);

  // Assert: los valores deben convertirse de kB a bytes (× 1024)
  const uint64_t expected_total     = static_cast<uint64_t>(total_kb)     * 1024;
  const uint64_t expected_available = static_cast<uint64_t>(available_kb) * 1024;
  const uint64_t expected_used      = expected_total - expected_available;

  EXPECT_EQ(info.total,     expected_total);
  EXPECT_EQ(info.available, expected_available);
  EXPECT_EQ(info.used,      expected_used);
}

// ---------------------------------------------------------------------------
// Test 2: se lanza std::runtime_error cuando la ruta no existe
// ---------------------------------------------------------------------------
TEST_F(RamUsageTest, LanzaRuntimeErrorParaRutaInexistente) {
  const std::string ruta_inexistente = std::string(::testing::TempDir()) + "/no_existe.txt";

  EXPECT_THROW(getRamUsage(ruta_inexistente), std::runtime_error);
}