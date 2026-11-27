#include <gtest/gtest.h>
#include <filesystem>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <set>
#include <SQLiteCpp/SQLiteCpp.h>
#include <ctime>
#include <limits>

#include "storage/storage.hpp"
#include "storage/schema.hpp"

namespace fs = std::filesystem;

static chuspapulse::core::Snapshot crearSnapshot(std::int64_t timestamp) {
    chuspapulse::core::Snapshot s;
    s.timestamp = timestamp;

    auto addMetric = [&](const std::string& name, const std::string& unit, double value) {
        chuspapulse::core::Metrica m;
        m.nombre    = name;
        m.unidad    = unit;
        m.valor     = value;
        m.timestamp = timestamp;
        s.metricas.push_back(m);
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
    chuspapulse::storage::Storage storage(dbPath_);
    auto resultado = storage.last();
    EXPECT_FALSE(resultado.has_value());
}

// 2. Base recién creada devuelve vector vacío en history()
TEST_F(StorageTest, Test_BaseVacia_HistorialDevuelveVectorVacio) {
    chuspapulse::storage::Storage storage(dbPath_);
    auto resultado = storage.history(0);
    EXPECT_TRUE(resultado.empty());
}

// 3. Insertar un snapshot y verificar que last() lo devuelve con todos sus campos
TEST_F(StorageTest, Test_GuardarYLeerUltimo) {
    chuspapulse::storage::Storage storage(dbPath_);
    auto snap = crearSnapshot(1000);
    storage.save(snap);

    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->timestamp, 1000);
    EXPECT_FALSE(resultado->metricas.empty());

    // Verificar que cpu.usage se guardó correctamente
    bool encontrado = false;
    for (const auto& m : resultado->metricas) {
        if (m.nombre == "cpu.usage") {
            EXPECT_DOUBLE_EQ(m.valor, 42.0);
            encontrado = true;
        }
    }
    EXPECT_TRUE(encontrado);
}

// 4. Insertar tres snapshots y verificar que last() devuelve el de mayor timestamp
TEST_F(StorageTest, Test_GuardarVarios_UltimoDevuelveElMasReciente) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(300));
    storage.save(crearSnapshot(200));

    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->timestamp, 300);
}

// 5. Insertar cinco snapshots, consultar un rango que cubra solo tres
TEST_F(StorageTest, Test_HistorialFiltraPorRango) {
    chuspapulse::storage::Storage storage(dbPath_);
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
    chuspapulse::storage::Storage storage(dbPath_);
    for (int i = 1; i <= 100; ++i) {
        storage.save(crearSnapshot(static_cast<std::int64_t>(i)));
    }

    auto resultado = storage.history(0, 0, 10);
    EXPECT_EQ(resultado.size(), 10u);
}

// 7. Insertar N snapshots y verificar que total() devuelve N
TEST_F(StorageTest, Test_TotalCuentaCorrectamente) {
    chuspapulse::storage::Storage storage(dbPath_);
    const int N = 7;
    for (int i = 1; i <= N; ++i) {
        storage.save(crearSnapshot(static_cast<std::int64_t>(i)));
    }

    EXPECT_EQ(storage.total(), static_cast<std::size_t>(N));
}

// 8. Persistencia: crear, guardar, destruir, reabrir y verificar que los datos siguen
TEST_F(StorageTest, Test_PersistenciaTrasReabrir) {
    {
        chuspapulse::storage::Storage storage(dbPath_);
        storage.save(crearSnapshot(9999));
    } // destruido aquí

    // Nueva instancia con el mismo archivo
    chuspapulse::storage::Storage storage2(dbPath_);
    auto resultado = storage2.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->timestamp, 9999);
}

// 9. Con setMaxEntries(3) se conservan solo los 3 snapshots más recientes (FIFO)
TEST_F(StorageTest, Test_SetMaxEntries_DescartaAntiguosFIFO) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.setMaxEntries(3);

    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(200));
    storage.save(crearSnapshot(300));
    storage.save(crearSnapshot(400));
    storage.save(crearSnapshot(500));

    EXPECT_EQ(storage.total(), 3u);

    auto resultado = storage.history(0);
    ASSERT_EQ(resultado.size(), 3u);
    EXPECT_EQ(resultado[0].timestamp, 300);
    EXPECT_EQ(resultado[1].timestamp, 400);
    EXPECT_EQ(resultado[2].timestamp, 500);
}

// 10. El esquema crea la tabla snapshots con las columnas que usan las consultas
TEST_F(StorageTest, Test_Esquema_CreaColumnasDeSnapshots) {
    SQLite::Database db(dbPath_, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    chuspapulse::storage::inicializarEsquema(db);

    std::set<std::string> columnas;
    SQLite::Statement query(db, "PRAGMA table_info(snapshots);");
    while (query.executeStep()) {
        columnas.insert(query.getColumn(1).getString());
    }

    const std::set<std::string> esperadas = {
        "timestamp", "cpu_usage", "cpu_cores",
        "memory_total", "memory_used", "memory_available",
        "disk_total", "disk_used", "disk_free",
        "network_rx_bytes", "network_tx_bytes"
    };
    EXPECT_EQ(columnas, esperadas);
}

// 11. El esquema crea la tabla alerts y los índices por timestamp
TEST_F(StorageTest, Test_Esquema_CreaTablaAlertasEIndices) {
    SQLite::Database db(dbPath_, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    chuspapulse::storage::inicializarEsquema(db);

    EXPECT_TRUE(db.tableExists("alerts"));

    std::set<std::string> indices;
    SQLite::Statement query(db,
        "SELECT name FROM sqlite_master WHERE type = 'index';");
    while (query.executeStep()) {
        indices.insert(query.getColumn(0).getString());
    }
    EXPECT_EQ(indices.count("idx_snapshots_timestamp"), 1u);
    EXPECT_EQ(indices.count("idx_alerts_timestamp"), 1u);
}

// 12. Inicializar el esquema dos veces no falla ni borra datos
TEST_F(StorageTest, Test_Esquema_EsIdempotente) {
    SQLite::Database db(dbPath_, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    chuspapulse::storage::inicializarEsquema(db);
    db.exec("INSERT INTO snapshots (timestamp) VALUES (1);");

    EXPECT_NO_THROW(chuspapulse::storage::inicializarEsquema(db));

    SQLite::Statement query(db, "SELECT COUNT(*) FROM snapshots;");
    ASSERT_TRUE(query.executeStep());
    EXPECT_EQ(query.getColumn(0).getInt64(), 1);
}

// 13. Guardar y recuperar conserva el valor y la unidad de todas las métricas
TEST_F(StorageTest, Test_GuardarYLeer_ConservaTodasLasMetricas) {
    chuspapulse::storage::Storage storage(dbPath_);
    auto original = crearSnapshot(1234);
    storage.save(original);

    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    ASSERT_EQ(resultado->metricas.size(), original.metricas.size());

    for (const auto& esperada : original.metricas) {
        bool encontrada = false;
        for (const auto& m : resultado->metricas) {
            if (m.nombre == esperada.nombre) {
                EXPECT_DOUBLE_EQ(m.valor, esperada.valor) << m.nombre;
                EXPECT_EQ(m.unidad, esperada.unidad) << m.nombre;
                EXPECT_EQ(m.timestamp, 1234) << m.nombre;
                encontrada = true;
            }
        }
        EXPECT_TRUE(encontrada) << esperada.nombre;
    }
}

// 14. exportToCSV escribe la cabecera y una fila por snapshot
TEST_F(StorageTest, Test_ExportarCSV_UnaFilaPorSnapshot) {
    const std::string csvPath = dbPath_ + ".csv";
    {
        chuspapulse::storage::Storage storage(dbPath_);
        storage.save(crearSnapshot(100));
        storage.save(crearSnapshot(200));
        storage.exportToCSV(csvPath);
    }

    std::ifstream archivo(csvPath);
    ASSERT_TRUE(archivo.is_open());

    std::string linea;
    std::getline(archivo, linea);
    EXPECT_EQ(linea.rfind("timestamp,cpu_usage,", 0), 0u);

    int filas = 0;
    while (std::getline(archivo, linea)) {
        if (!linea.empty()) ++filas;
    }
    EXPECT_EQ(filas, 2);

    archivo.close();
    fs::remove(csvPath);
}

// 15. exportToCSV() a una carpeta inexistente lanza error y no deja archivos
TEST_F(StorageTest, Test_ExportarCSV_RutaInvalidaLanzaError) {
    chuspapulse::storage::Storage storage(dbPath_);
    const std::string ruta = "carpeta_que_no_existe_1533/salida.csv";

    EXPECT_THROW(storage.exportToCSV(ruta), std::runtime_error);
    EXPECT_FALSE(fs::exists(ruta));
    EXPECT_FALSE(fs::exists(ruta + ".tmp"));
}

// 16. Si la consulta falla a mitad de la exportación, se informa el error,
//     no queda un CSV parcial y el archivo previo de destino no se pisa
TEST_F(StorageTest, Test_ExportarCSV_FalloDeConsultaNoDejaCSVParcial) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));

    const std::string ruta = "export_1533_test.csv";
    {
        std::ofstream previo(ruta);
        previo << "contenido previo\n";
    }

    // Forzar el fallo de la consulta desde otra conexión
    {
        SQLite::Database otra(dbPath_, SQLite::OPEN_READWRITE);
        otra.exec("DROP TABLE snapshots;");
    }

    EXPECT_THROW(storage.exportToCSV(ruta), std::runtime_error);
    EXPECT_FALSE(fs::exists(ruta + ".tmp"));

    std::ifstream leido(ruta);
    std::stringstream contenido;
    contenido << leido.rdbuf();
    EXPECT_EQ(contenido.str(), "contenido previo\n");

    leido.close();
    fs::remove(ruta);
}

// 17. Si no se puede mover el temporal al destino (el destino es una
//     carpeta), se informa el error y no queda el .tmp
TEST_F(StorageTest, Test_ExportarCSV_FalloAlMoverAlDestinoLanzaError) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));

    const std::string ruta = "destino_carpeta_1533";
    fs::create_directory(ruta);

    EXPECT_THROW(storage.exportToCSV(ruta), std::runtime_error);
    EXPECT_FALSE(fs::exists(ruta + ".tmp"));
    EXPECT_TRUE(fs::is_directory(ruta));

    fs::remove_all(ruta);
}

// ---------------------------------------------------------------------------
// Casos borde adicionales
// ---------------------------------------------------------------------------

static double valorDe(const chuspapulse::core::Snapshot& s, const std::string& nombre) {
    for (const auto& m : s.metricas) {
        if (m.nombre == nombre) return m.valor;
    }
    ADD_FAILURE() << "no se encontró la métrica " << nombre;
    return -1.0;
}

static void fijarMetrica(chuspapulse::core::Snapshot& s, const std::string& nombre, double valor) {
    for (auto& m : s.metricas) {
        if (m.nombre == nombre) m.valor = valor;
    }
}

// 18. history() con rango invertido (from > until) devuelve vacío
TEST_F(StorageTest, Test_Historial_RangoInvertidoDevuelveVacio) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(200));
    storage.save(crearSnapshot(300));

    auto resultado = storage.history(300, 100);
    EXPECT_TRUE(resultado.empty());
}

// 19. Los extremos del rango son inclusivos: from == until devuelve ese timestamp
TEST_F(StorageTest, Test_Historial_ExtremosInclusivos) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(200));
    storage.save(crearSnapshot(300));

    auto resultado = storage.history(200, 200);
    ASSERT_EQ(resultado.size(), 1u);
    EXPECT_EQ(resultado[0].timestamp, 200);
}

// 20. history() con limite 0 devuelve vacío aunque haya datos
TEST_F(StorageTest, Test_Historial_LimiteCeroDevuelveVacio) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(200));

    auto resultado = storage.history(0, 0, 0);
    EXPECT_TRUE(resultado.empty());
}

// 21. until = 0 significa "sin límite superior", incluso con timestamps muy grandes
TEST_F(StorageTest, Test_Historial_UntilCeroSinLimiteSuperior) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(200));
    storage.save(crearSnapshot(9000000000LL));

    auto resultado = storage.history(0, 0);
    ASSERT_EQ(resultado.size(), 3u);
    EXPECT_EQ(resultado[2].timestamp, 9000000000LL);
}

// 22. Timestamps negativos se guardan y se filtran correctamente
TEST_F(StorageTest, Test_TimestampsNegativos_SeGuardanYFiltran) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(-50));
    storage.save(crearSnapshot(-10));

    auto ultimo = storage.last();
    ASSERT_TRUE(ultimo.has_value());
    EXPECT_EQ(ultimo->timestamp, -10);

    auto resultado = storage.history(-100, -20);
    ASSERT_EQ(resultado.size(), 1u);
    EXPECT_EQ(resultado[0].timestamp, -50);
}

// 23. El timestamp máximo de 64 bits no se trunca
TEST_F(StorageTest, Test_TimestampMaximoInt64_SeConserva) {
    chuspapulse::storage::Storage storage(dbPath_);
    const std::int64_t maximo = std::numeric_limits<std::int64_t>::max();
    storage.save(crearSnapshot(maximo));

    auto ultimo = storage.last();
    ASSERT_TRUE(ultimo.has_value());
    EXPECT_EQ(ultimo->timestamp, maximo);
}

// 24. Timestamps duplicados: se guardan todos y history() los devuelve todos
TEST_F(StorageTest, Test_TimestampsDuplicados_SeConservanTodos) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(100));

    EXPECT_EQ(storage.total(), 3u);
    EXPECT_EQ(storage.history(100, 100).size(), 3u);
}

// 25. Un snapshot sin métricas se guarda con todos los campos en 0
TEST_F(StorageTest, Test_SnapshotSinMetricas_GuardaCeros) {
    chuspapulse::storage::Storage storage(dbPath_);
    chuspapulse::core::Snapshot vacio;
    vacio.timestamp = 500;
    storage.save(vacio);

    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->timestamp, 500);
    ASSERT_EQ(resultado->metricas.size(), 10u);
    for (const auto& m : resultado->metricas) {
        EXPECT_DOUBLE_EQ(m.valor, 0.0) << m.nombre;
    }
}

// 26. Un snapshot con métricas parciales guarda 0 en las que faltan
TEST_F(StorageTest, Test_SnapshotParcial_FaltantesQuedanEnCero) {
    chuspapulse::storage::Storage storage(dbPath_);
    chuspapulse::core::Snapshot parcial;
    parcial.timestamp = 700;
    chuspapulse::core::Metrica m;
    m.nombre    = "memory.total";
    m.unidad    = "bytes";
    m.valor     = 1000.0;
    m.timestamp = 700;
    parcial.metricas.push_back(m);
    storage.save(parcial);

    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_DOUBLE_EQ(valorDe(*resultado, "memory.total"), 1000.0);
    EXPECT_DOUBLE_EQ(valorDe(*resultado, "cpu.usage"), 0.0);
    EXPECT_DOUBLE_EQ(valorDe(*resultado, "disk.free"), 0.0);
}

// 27. Los valores enteros (cores, bytes) se truncan al guardar valores con decimales
TEST_F(StorageTest, Test_ValoresDecimales_SeTruncanEnColumnasEnteras) {
    chuspapulse::storage::Storage storage(dbPath_);
    auto snap = crearSnapshot(10);
    fijarMetrica(snap, "cpu.cores", 3.7);
    fijarMetrica(snap, "memory.used", 1.9);
    storage.save(snap);

    auto resultado = storage.last();
    ASSERT_TRUE(resultado.has_value());
    EXPECT_DOUBLE_EQ(valorDe(*resultado, "cpu.cores"), 3.0);
    EXPECT_DOUBLE_EQ(valorDe(*resultado, "memory.used"), 1.0);
}

// 28. Un cpu.usage NaN viola NOT NULL: save() lanza error y no se guarda nada
TEST_F(StorageTest, Test_ValorNaN_LanzaErrorYNoGuarda) {
    chuspapulse::storage::Storage storage(dbPath_);
    auto snap = crearSnapshot(10);
    fijarMetrica(snap, "cpu.usage", std::numeric_limits<double>::quiet_NaN());

    EXPECT_THROW(storage.save(snap), std::exception);
    EXPECT_EQ(storage.total(), 0u);
}

// 29. setMaxEntries(0) vacía la base y no retiene nuevos snapshots
TEST_F(StorageTest, Test_SetMaxEntriesCero_VaciaLaBase) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.save(crearSnapshot(100));
    storage.save(crearSnapshot(200));
    storage.save(crearSnapshot(300));

    storage.setMaxEntries(0);
    EXPECT_EQ(storage.total(), 0u);
    EXPECT_FALSE(storage.last().has_value());

    storage.save(crearSnapshot(400));
    EXPECT_EQ(storage.total(), 0u);
}

// 30. Reducir el límite sobre datos ya guardados poda de inmediato los más antiguos
TEST_F(StorageTest, Test_SetMaxEntries_ReducirPodaDatosExistentes) {
    chuspapulse::storage::Storage storage(dbPath_);
    for (int i = 1; i <= 5; ++i) {
        storage.save(crearSnapshot(static_cast<std::int64_t>(i) * 100));
    }

    storage.setMaxEntries(2);

    auto resultado = storage.history(0);
    ASSERT_EQ(resultado.size(), 2u);
    EXPECT_EQ(resultado[0].timestamp, 400);
    EXPECT_EQ(resultado[1].timestamp, 500);
}

// 31. La poda descarta por timestamp más antiguo, no por orden de inserción
TEST_F(StorageTest, Test_SetMaxEntries_PodaPorTimestampNoPorInsercion) {
    chuspapulse::storage::Storage storage(dbPath_);
    storage.setMaxEntries(2);
    storage.save(crearSnapshot(300));
    storage.save(crearSnapshot(400));
    storage.save(crearSnapshot(100));  // el más antiguo, aunque se insertó al final

    auto resultado = storage.history(0);
    ASSERT_EQ(resultado.size(), 2u);
    EXPECT_EQ(resultado[0].timestamp, 300);
    EXPECT_EQ(resultado[1].timestamp, 400);
}

// 32. Abrir una base en una carpeta inexistente lanza error
TEST_F(StorageTest, Test_Constructor_RutaInvalidaLanzaError) {
    EXPECT_THROW(
        chuspapulse::storage::Storage storage("carpeta_que_no_existe_1705/base.db"),
        std::runtime_error);
}

// 33. exportToCSV con base vacía escribe solo la cabecera
TEST_F(StorageTest, Test_ExportarCSV_BaseVaciaSoloCabecera) {
    const std::string csvPath = dbPath_ + ".csv";
    chuspapulse::storage::Storage storage(dbPath_);
    storage.exportToCSV(csvPath);

    std::ifstream archivo(csvPath);
    ASSERT_TRUE(archivo.is_open());

    std::string linea;
    ASSERT_TRUE(static_cast<bool>(std::getline(archivo, linea)));
    EXPECT_EQ(linea.rfind("timestamp,cpu_usage,", 0), 0u);
    EXPECT_FALSE(static_cast<bool>(std::getline(archivo, linea)));

    archivo.close();
    fs::remove(csvPath);
}

// 34. exportToCSV ordena por timestamp aunque se hayan insertado desordenados
TEST_F(StorageTest, Test_ExportarCSV_OrdenaPorTimestamp) {
    const std::string csvPath = dbPath_ + ".csv";
    {
        chuspapulse::storage::Storage storage(dbPath_);
        storage.save(crearSnapshot(300));
        storage.save(crearSnapshot(100));
        storage.save(crearSnapshot(200));
        storage.exportToCSV(csvPath);
    }

    std::ifstream archivo(csvPath);
    ASSERT_TRUE(archivo.is_open());

    std::string linea;
    std::getline(archivo, linea);  // cabecera

    std::vector<std::string> primeras;
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;
        primeras.push_back(linea.substr(0, linea.find(',')));
    }
    ASSERT_EQ(primeras.size(), 3u);
    EXPECT_EQ(primeras[0], "100");
    EXPECT_EQ(primeras[1], "200");
    EXPECT_EQ(primeras[2], "300");

    archivo.close();
    fs::remove(csvPath);
}

// 35. getPromedio sin snapshots devuelve las 10 métricas en 0
TEST_F(StorageTest, Test_Promedio_SinDatosDevuelveCeros) {
    chuspapulse::storage::Storage storage(dbPath_);
    auto promedio = storage.getPromedio(60);

    ASSERT_EQ(promedio.metricas.size(), 10u);
    for (const auto& m : promedio.metricas) {
        EXPECT_DOUBLE_EQ(m.valor, 0.0) << m.nombre;
    }
}

// 36. getPromedio promedia solo los snapshots dentro de la ventana
TEST_F(StorageTest, Test_Promedio_IgnoraSnapshotsFueraDeVentana) {
    chuspapulse::storage::Storage storage(dbPath_);
    const std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    auto a = crearSnapshot(ahora);
    fijarMetrica(a, "cpu.usage", 10.0);
    auto b = crearSnapshot(ahora);
    fijarMetrica(b, "cpu.usage", 30.0);
    auto viejo = crearSnapshot(ahora - 1000);
    fijarMetrica(viejo, "cpu.usage", 90.0);

    storage.save(a);
    storage.save(b);
    storage.save(viejo);

    auto promedio = storage.getPromedio(60);
    EXPECT_DOUBLE_EQ(valorDe(promedio, "cpu.usage"), 20.0);
}
