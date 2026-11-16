#include <ctime>
#include <gtest/gtest.h>
#include <filesystem>
#include <cstdint>
#include <string>

#include "storage/storage.hpp"

namespace fs = std::filesystem;

// Snapshot auxiliar para pruebas de promedio
static chuspapulse::core::Snapshot crearSnapshotCPU(
    std::int64_t timestamp,
    double cpu)
{
    chuspapulse::core::Snapshot s;
    s.timestamp = timestamp;

    chuspapulse::core::Metrica m;
    m.nombre = "cpu.usage";
    m.unidad = "percent";
    m.valor = cpu;
    m.timestamp = timestamp;

    s.metricas.push_back(m);

    return s;
}

class StoragePromedioTest : public ::testing::Test {
protected:
    std::string dbPath_;

    void SetUp() override {
        dbPath_ =
            std::string(::testing::UnitTest::GetInstance()
                            ->current_test_info()
                            ->name()) +
            "_test.db";
    }

    void TearDown() override {
        if (fs::exists(dbPath_)) {
            fs::remove(dbPath_);
        }
    }
};

// 1. Promedio de 30,60,90 debe dar 60
TEST_F(StoragePromedioTest, PromedioCPU)
{
    chuspapulse::storage::Storage storage(dbPath_);

    std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    storage.save(crearSnapshotCPU(ahora - 3, 30.0));
    storage.save(crearSnapshotCPU(ahora - 2, 60.0));
    storage.save(crearSnapshotCPU(ahora - 1, 90.0));

    auto promedio = storage.getPromedio(10);

    bool encontrado = false;

    for (const auto& m : promedio.metricas) {
        if (m.nombre == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.valor, 60.0);
            encontrado = true;
        }
    }

    EXPECT_TRUE(encontrado);
}

// 2. Ventana pequeña: solo debe considerar el snapshot más reciente
TEST_F(StoragePromedioTest, VentanaSinDatosAntiguos)
{
    chuspapulse::storage::Storage storage(dbPath_);

    std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    storage.save(crearSnapshotCPU(ahora - 100, 30.0));
    storage.save(crearSnapshotCPU(ahora - 50, 60.0));
    storage.save(crearSnapshotCPU(ahora - 1, 90.0));

    auto promedio = storage.getPromedio(5);

    bool encontrado = false;

    for (const auto& m : promedio.metricas) {
        if (m.nombre == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.valor, 90.0);
            encontrado = true;
        }
    }

    EXPECT_TRUE(encontrado);
}

// 3. Sin snapshots debe devolver cpu = 0
TEST_F(StoragePromedioTest, PromedioMultiplesMetricas)
{
    chuspapulse::storage::Storage storage(dbPath_);

    auto promedio = storage.getPromedio(60);

    bool encontrado = false;

    for (const auto& m : promedio.metricas) {
        if (m.nombre == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.valor, 0.0);
            encontrado = true;
        }
    }

    EXPECT_TRUE(encontrado);
}

TEST_F(StoragePromedioTest, MetricaDesconocidaNoAfectaPromedio)
{
    chuspapulse::storage::Storage storage(dbPath_);
    auto ahora = static_cast<std::int64_t>(std::time(nullptr));

    auto snapshot = crearSnapshotCPU(ahora - 1, 40.0);

    chuspapulse::core::Metrica desconocida;
    desconocida.nombre = "metrica.invalida";
    desconocida.unidad = "unknown";
    desconocida.valor = 999.0;
    desconocida.timestamp = ahora - 1;

    snapshot.metricas.push_back(desconocida);
    storage.save(snapshot);

    auto promedio = storage.getPromedio(10);

    bool encontrado = false;
    for (const auto& m : promedio.metricas) {
        EXPECT_NE(m.nombre, "metrica.invalida");

        if (m.nombre == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.valor, 40.0);
            encontrado = true;
        }
    }

    EXPECT_TRUE(encontrado);
}

TEST_F(StoragePromedioTest, ValorLimiteCPU)
{
    chuspapulse::storage::Storage storage(dbPath_);
    auto ahora = static_cast<std::int64_t>(std::time(nullptr));

    storage.save(crearSnapshotCPU(ahora - 1, 100.0));

    auto promedio = storage.getPromedio(10);

    bool encontrado = false;
    for (const auto& m : promedio.metricas) {
        if (m.nombre == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.valor, 100.0);
            encontrado = true;
        }
    }

    EXPECT_TRUE(encontrado);
}

TEST_F(StoragePromedioTest, VentanaSinRegistrosDevuelveCeros)
{
    chuspapulse::storage::Storage storage(dbPath_);
    auto ahora = static_cast<std::int64_t>(std::time(nullptr));

    storage.save(crearSnapshotCPU(ahora - 100, 75.0));

    auto promedio = storage.getPromedio(5);

    ASSERT_EQ(promedio.metricas.size(), 10u);

    for (const auto& m : promedio.metricas) {
        EXPECT_DOUBLE_EQ(m.valor, 0.0);
    }
}

TEST_F(StoragePromedioTest, MetricaAusenteSeGuardaComoCero)
{
    chuspapulse::storage::Storage storage(dbPath_);
    auto ahora = static_cast<std::int64_t>(std::time(nullptr));

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = ahora - 1;

    storage.save(snapshot);

    auto promedio = storage.getPromedio(10);

    bool encontrado = false;
    for (const auto& m : promedio.metricas) {
        if (m.nombre == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.valor, 0.0);
            encontrado = true;
        }
    }

    EXPECT_TRUE(encontrado);
}
