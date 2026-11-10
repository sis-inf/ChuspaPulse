#include <gtest/gtest.h>
#include "collectors/disk/disk_usage.cpp"

namespace {

// Tolerancia de 1 MB para la verificación de consistencia
constexpr uint64_t TOLERANCIA = 1048576;

/**
 * @brief Verifica que el espacio total del disco es mayor a cero.
 */
TEST(DiskUsageTest, TotalEsMayorACero) {
    DiskUsage uso = getDiskUsage();
    ASSERT_GT(uso.total, static_cast<uint64_t>(0));
}

/**
 * @brief Verifica que el espacio libre no es negativo.
 */
TEST(DiskUsageTest, LibreNoEsNegativo) {
    DiskUsage uso = getDiskUsage();
    ASSERT_GE(uso.libre, static_cast<uint64_t>(0));
}

/**
 * @brief Verifica que el espacio usado no es negativo.
 */
TEST(DiskUsageTest, UsadoNoEsNegativo) {
    DiskUsage uso = getDiskUsage();
    ASSERT_GE(uso.usado, static_cast<uint64_t>(0));
}

/**
 * @brief Verifica que usado + libre no supera total más la tolerancia de 1 MB.
 */
TEST(DiskUsageTest, ConsistenciaUsadoMasLibre) {
    DiskUsage uso = getDiskUsage();
    ASSERT_LE(uso.usado + uso.libre, uso.total + TOLERANCIA);
}

// ---------------------------------------------------------------------------
// Porcentajes calculados sobre la salida real del recolector
// ---------------------------------------------------------------------------

/**
 * @brief Los porcentajes de uso y de espacio libre estan entre 0 y 100.
 */
TEST(DiskUsageTest, PorcentajesDelRecolectorRealEstanEntreCeroYCien) {
    const DiskUsage uso = getDiskUsage();
    ASSERT_GT(uso.total, static_cast<uint64_t>(0));

    const double usado = (static_cast<double>(uso.usado) * 100.0) /
                         static_cast<double>(uso.total);
    const double libre = (static_cast<double>(uso.libre) * 100.0) /
                         static_cast<double>(uso.total);

    EXPECT_GE(usado, 0.0);
    EXPECT_LE(usado, 100.0);
    EXPECT_GE(libre, 0.0);
    EXPECT_LE(libre, 100.0);
}

/**
 * @brief Los porcentajes de uso y de espacio libre suman 100 por ciento.
 */
TEST(DiskUsageTest, PorcentajesDelRecolectorRealSumanCien) {
    const DiskUsage uso = getDiskUsage();
    ASSERT_GT(uso.total, static_cast<uint64_t>(0));

    const double usado = (static_cast<double>(uso.usado) * 100.0) /
                         static_cast<double>(uso.total);
    const double libre = (static_cast<double>(uso.libre) * 100.0) /
                         static_cast<double>(uso.total);

    EXPECT_NEAR(100.0, usado + libre, 0.000001);
}

/**
 * @brief El recolector calcula el espacio usado como total menos libre.
 */
TEST(DiskUsageTest, RecolectorRealCalculaUsadoComoTotalMenosLibre) {
    const DiskUsage uso = getDiskUsage();

    EXPECT_EQ(uso.total - uso.libre, uso.usado);
}

/**
 * @brief Una ruta inexistente hace fallar la lectura con una excepcion.
 */
TEST(DiskUsageTest, RutaInexistenteLanzaRuntimeError) {
    EXPECT_THROW(getDiskUsage("/chuspapulse/ruta/que/no/existe"),
                 std::runtime_error);
}

// ---------------------------------------------------------------------------
// El recolector reporta solo el sistema de archivos raiz
// ---------------------------------------------------------------------------

/**
 * @brief El recolector se identifica con el nombre disk.
 */
TEST(DiskCollectorTest, NombreDelRecolectorEsDisk) {
    chuspapulse::collectors::disk::DiskCollector colector;

    EXPECT_EQ("disk", colector.nombre());
}

/**
 * @brief El recolector entrega exactamente tres metricas.
 */
TEST(DiskCollectorTest, EntregaExactamenteTresMetricas) {
    chuspapulse::collectors::disk::DiskCollector colector;

    const std::vector<chuspapulse::core::Metrica> metricas = colector.recolectar();

    ASSERT_EQ(static_cast<size_t>(3), metricas.size());
}

/**
 * @brief Las tres metricas son las del sistema de archivos raiz, en bytes.
 */
TEST(DiskCollectorTest, EntregaLasMetricasDelSistemaDeArchivosRaiz) {
    chuspapulse::collectors::disk::DiskCollector colector;

    const std::vector<chuspapulse::core::Metrica> metricas = colector.recolectar();

    ASSERT_EQ(static_cast<size_t>(3), metricas.size());
    EXPECT_EQ("disk.total", metricas[0].nombre);
    EXPECT_EQ("disk.used", metricas[1].nombre);
    EXPECT_EQ("disk.free", metricas[2].nombre);

    for (const chuspapulse::core::Metrica& metrica : metricas) {
        EXPECT_EQ("bytes", metrica.unidad);
    }
}

/**
 * @brief Ningun punto de montaje virtual aparece entre las metricas entregadas.
 */
TEST(DiskCollectorTest, NoEntregaMetricasDeMontajesVirtuales) {
    chuspapulse::collectors::disk::DiskCollector colector;

    const std::vector<chuspapulse::core::Metrica> metricas = colector.recolectar();

    for (const chuspapulse::core::Metrica& metrica : metricas) {
        EXPECT_EQ(std::string::npos, metrica.nombre.find("tmpfs"));
        EXPECT_EQ(std::string::npos, metrica.nombre.find("proc"));
        EXPECT_EQ(std::string::npos, metrica.nombre.find("sysfs"));
        EXPECT_EQ(std::string::npos, metrica.nombre.find("shm"));
    }
}

/**
 * @brief Los valores entregados coinciden con los que devuelve el recolector.
 */
TEST(DiskCollectorTest, LosValoresCoincidenConLaLecturaDelDisco) {
    chuspapulse::collectors::disk::DiskCollector colector;

    const std::vector<chuspapulse::core::Metrica> metricas = colector.recolectar();

    ASSERT_EQ(static_cast<size_t>(3), metricas.size());
    EXPECT_LE(metricas[1].valor + metricas[2].valor,
              metricas[0].valor + static_cast<double>(TOLERANCIA));
}
} // namespace
