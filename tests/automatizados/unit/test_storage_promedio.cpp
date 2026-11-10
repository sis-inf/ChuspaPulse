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
    m.name = "cpu.usage";
    m.unit = "percent";
    m.value = cpu;
    m.timestamp = timestamp;

    s.metrics.push_back(m);

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

    for (const auto& m : promedio.metrics) {
        if (m.name == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.value, 60.0);
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

    for (const auto& m : promedio.metrics) {
        if (m.name == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.value, 90.0);
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

    for (const auto& m : promedio.metrics) {
        if (m.name == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.value, 0.0);
            encontrado = true;
        }
    }

    EXPECT_TRUE(encontrado);
}
