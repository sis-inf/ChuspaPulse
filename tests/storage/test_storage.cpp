#include <gtest/gtest.h>
#include <filesystem>
#include <cstdint>
#include <limits>
#include <string>

#include "storage/storage.hpp"

namespace fs = std::filesystem;

static pulso::core::Snapshot crearSnapshot(std::int64_t timestamp) {
    pulso::core::Snapshot s;
    s.timestamp = timestamp;

    auto addMetric = [&](const std::string& name, const std::string& unit, double value) {
        pulso::core::Metrica m;
        m.name      = name;
        m.unit      = unit;
        m.value     = value;
        m.timestamp = timestamp;
        s.metrics.push_back(m);
    };

    addMetric("cpu.usage",        "percent", 42.0);
    addMetric("cpu.cores",        "cores",   4.0);
    addMetric("memory.total",     "bytes",   8000000000.0);
    addMetric("memory.used",      "bytes",   3000000000.0);
    addMetric("memory.available", "bytes",   5000000000.0);
    addMetric("disk.total",       "bytes",   256000000000.0);
    addMetric("disk.used",        "bytes",   120000000000.0);
    addMetric("disk.free",        "bytes",   136000000000.0);
    addMetric("network.rx_bytes", "bytes",   1024.0);
    addMetric("network.tx_bytes", "bytes",   512.0);

    return s;
}

//Fixture base 

class StorageTest : public ::testing::Test {
protected:
    std::string dbPath_;

    void SetUp() override {
        // Archivo temporal único por test
        dbPath_ = std::string(::testing::UnitTest::GetInstance()
                                  ->current_test_info()->name()) + "_test.db";
    }

    void TearDown() override {
        // Limpiar archivo temporal
        if (fs::exists(dbPath_)) {
            fs::remove(dbPath_);
        }
    }
};

//Tests

// 1. Base recién creada devuelve std::nullopt en last()
TEST_F(StorageTest, Test_BaseVacia_UltimoDevuelveNullopt) {
    pulso::storage::Storage storage(dbPath_);
    auto resultado = storage.last();
    EXPECT_FALSE(resultado.has_value());
}

// 2. Base recién creada devuelve vector vacío en history()
TEST_F(StorageTest, Test_BaseVacia_HistorialDevuelveVectorVacio) {
    pulso::storage::Storage storage(dbPath_);
    auto resultado = storage.history(0);
    EXPECT_TRUE(resultado.empty());
}

// 3. Insertar un snapshot y verificar que last() lo devuelve con todos sus campos
TEST_F(StorageTest, Test_GuardarYLeerUltimo) {
    pulso::storage::Storage storage(dbPath_);
    auto snap = crearSnapshot(1000);
    storage.save(snap);

    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->timestamp, 1000);
    EXPECT_FALSE(resultado->metrics.empty());

    // Verificar que cpu.usage se guardó correctamente
    bool encontrado = false;
    for (const auto& m : resultado->metrics) {
        if (m.name == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.value, 42.0);
            encontrado = true;
        }
    }
    EXPECT_TRUE(encontrado);
}

// 4. Insertar tres snapshots y verificar que last() devuelve el de mayor timestamp
TEST_F(StorageTest, Test_GuardarVarios_UltimoDevuelveElMasReciente) {
    pulso::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(300));
    storage.save(crearSnapshot(200));

    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->timestamp, 300);
}

// 5. Insertar cinco snapshots, consultar un rango que cubra solo tres
TEST_F(StorageTest, Test_HistorialFiltraPorRango) {
    pulso::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(200));
    storage.save(crearSnapshot(300));
    storage.save(crearSnapshot(400));
    storage.save(crearSnapshot(500));

    // Rango que cubre solo timestamps 200, 300, 400
    auto resultado = storage.history(200, 400);
    ASSERT_EQ(resultado.size(), 3u);
    EXPECT_EQ(resultado[0].timestamp, 200);
    EXPECT_EQ(resultado[1].timestamp, 300);
    EXPECT_EQ(resultado[2].timestamp, 400);
}

// 6. Insertar 100 snapshots y consultar con limite=10, verificar que devuelve 10
TEST_F(StorageTest, Test_HistorialRespetalimite) {
    pulso::storage::Storage storage(dbPath_);
    for (int i = 1; i <= 100; ++i) {
        storage.save(crearSnapshot(static_cast<std::int64_t>(i)));
    }

    auto resultado = storage.history(0, 0, 10);
    EXPECT_EQ(resultado.size(), 10u);
}

// 7. Insertar N snapshots y verificar que total() devuelve N
TEST_F(StorageTest, Test_TotalCuentaCorrectamente) {
    pulso::storage::Storage storage(dbPath_);
    const int N = 7;
    for (int i = 1; i <= N; ++i) {
        storage.save(crearSnapshot(static_cast<std::int64_t>(i)));
    }

    EXPECT_EQ(storage.total(), static_cast<std::size_t>(N));
}

// 8. Persistencia: crear, guardar, destruir, reabrir y verificar que los datos siguen
TEST_F(StorageTest, Test_PersistenciaTrasReabrir) {
    {
        pulso::storage::Storage storage(dbPath_);
        storage.save(crearSnapshot(9999));
    } // destruido aquí

    // Nueva instancia con el mismo archivo
    pulso::storage::Storage storage2(dbPath_);
    auto resultado = storage2.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->timestamp, 9999);
}

// 9. setMaxEntries(0) con la base vacía no falla
TEST_F(StorageTest, Test_MaximoCero_BaseVacia_NoLanza) {
    pulso::storage::Storage storage(dbPath_);

    EXPECT_NO_THROW(storage.setMaxEntries(0));
    EXPECT_EQ(storage.total(), 0u);
}

// 10. setMaxEntries(0) conserva las entradas existentes
TEST_F(StorageTest, Test_MaximoCero_ConservaEntradasExistentes) {
    pulso::storage::Storage storage(dbPath_);
    for (int i = 1; i <= 5; ++i) {
        storage.save(crearSnapshot(static_cast<std::int64_t>(i)));
    }

    storage.setMaxEntries(0);

    EXPECT_EQ(storage.total(), 5u);
}

// 11. Con límite 0, save() sigue guardando sin eliminar nada
TEST_F(StorageTest, Test_MaximoCero_SaveNoElimina) {
    pulso::storage::Storage storage(dbPath_);
    storage.setMaxEntries(0);

    for (int i = 1; i <= 5; ++i) {
        EXPECT_NO_THROW(storage.save(crearSnapshot(static_cast<std::int64_t>(i))));
    }

    EXPECT_EQ(storage.total(), 5u);
    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->timestamp, 5);
}

// 12. Pasar de sin límite a un límite concreto purga con política FIFO
TEST_F(StorageTest, Test_MaximoCero_LuegoLimitar_PurgaLosAntiguos) {
    pulso::storage::Storage storage(dbPath_);
    storage.setMaxEntries(0);
    for (int i = 1; i <= 5; ++i) {
        storage.save(crearSnapshot(static_cast<std::int64_t>(i)));
    }

    storage.setMaxEntries(3);

    auto resultado = storage.history(0);
    ASSERT_EQ(resultado.size(), 3u);
    EXPECT_EQ(resultado[0].timestamp, 3);
    EXPECT_EQ(resultado[2].timestamp, 5);
}

// 13. Límite 1: solo se conserva el snapshot más reciente
TEST_F(StorageTest, Test_MaximoUno_ConservaSoloElMasReciente) {
    pulso::storage::Storage storage(dbPath_);
    storage.setMaxEntries(1);

    storage.save(crearSnapshot(10));
    storage.save(crearSnapshot(20));
    storage.save(crearSnapshot(30));

    EXPECT_EQ(storage.total(), 1u);
    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->timestamp, 30);
}

// 14. Límite 2: se descartan los más antiguos (FIFO)
TEST_F(StorageTest, Test_MaximoDos_DescartaLosMasAntiguos) {
    pulso::storage::Storage storage(dbPath_);
    storage.setMaxEntries(2);

    for (int i = 1; i <= 4; ++i) {
        storage.save(crearSnapshot(static_cast<std::int64_t>(i)));
    }

    auto resultado = storage.history(0);
    ASSERT_EQ(resultado.size(), 2u);
    EXPECT_EQ(resultado[0].timestamp, 3);
    EXPECT_EQ(resultado[1].timestamp, 4);
}

// 15. Un límite mayor que la cantidad de entradas no elimina nada
TEST_F(StorageTest, Test_MaximoMayorQueTotal_NoEliminaNada) {
    pulso::storage::Storage storage(dbPath_);
    for (int i = 1; i <= 3; ++i) {
        storage.save(crearSnapshot(static_cast<std::int64_t>(i)));
    }

    storage.setMaxEntries(10);

    EXPECT_EQ(storage.total(), 3u);
}

// 16. El valor máximo representable no provoca errores ni borrados
TEST_F(StorageTest, Test_MaximoEnteroMaximo_NoEliminaNada) {
    pulso::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(1));

    EXPECT_NO_THROW(storage.setMaxEntries(std::numeric_limits<std::size_t>::max()));
    EXPECT_EQ(storage.total(), 1u);
}