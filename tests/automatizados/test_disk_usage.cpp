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
// Particiones simuladas: cálculo de porcentajes
// ---------------------------------------------------------------------------

/**
 * @brief Construye una estructura de uso de disco simulada.
 * @param total Espacio total en bytes.
 * @param usado Espacio usado en bytes.
 */
DiskUsage crearUsoSimulado(uint64_t total, uint64_t usado) {
    DiskUsage uso{};
    uso.total = total;
    uso.usado = usado;
    uso.libre = total - usado;
    return uso;
}

/// @brief Porcentaje de espacio ocupado sobre el total.
double porcentajeUsado(const DiskUsage& uso) {
    if (uso.total == 0) {
        return 0.0;
    }
    return (static_cast<double>(uso.usado) * 100.0) / static_cast<double>(uso.total);
}

/// @brief Porcentaje de espacio libre sobre el total.
double porcentajeLibre(const DiskUsage& uso) {
    if (uso.total == 0) {
        return 0.0;
    }
    return (static_cast<double>(uso.libre) * 100.0) / static_cast<double>(uso.total);
}

/**
 * @brief Una partición llena reporta 100 % ocupado y 0 % libre.
 */
TEST(DiskUsageTest, ParticionLlenaReportaCienPorCientoOcupado) {
    const DiskUsage uso = crearUsoSimulado(1000, 1000);
    EXPECT_DOUBLE_EQ(100.0, porcentajeUsado(uso));
    EXPECT_DOUBLE_EQ(0.0, porcentajeLibre(uso));
}

/**
 * @brief Una partición vacía reporta 0 % ocupado y 100 % libre.
 */
TEST(DiskUsageTest, ParticionVaciaReportaCienPorCientoLibre) {
    const DiskUsage uso = crearUsoSimulado(1000, 0);
    EXPECT_DOUBLE_EQ(0.0, porcentajeUsado(uso));
    EXPECT_DOUBLE_EQ(100.0, porcentajeLibre(uso));
}

/**
 * @brief Una partición ocupada a la mitad reporta 50 % en ambos porcentajes.
 */
TEST(DiskUsageTest, ParticionAMitadReportaCincuentaPorCiento) {
    const DiskUsage uso = crearUsoSimulado(2048, 1024);
    EXPECT_DOUBLE_EQ(50.0, porcentajeUsado(uso));
    EXPECT_DOUBLE_EQ(50.0, porcentajeLibre(uso));
}

/**
 * @brief Los porcentajes de usado y libre siempre suman 100 %.
 */
TEST(DiskUsageTest, PorcentajesSumanCien) {
    const DiskUsage casos[] = {
        crearUsoSimulado(1000, 0),
        crearUsoSimulado(1000, 250),
        crearUsoSimulado(1000, 999),
        crearUsoSimulado(1000, 1000),
    };

    for (const DiskUsage& uso : casos) {
        EXPECT_DOUBLE_EQ(100.0, porcentajeUsado(uso) + porcentajeLibre(uso));
    }
}

/**
 * @brief Una partición de tamaño cero no produce división por cero.
 */
TEST(DiskUsageTest, ParticionSinTamanoReportaCeroPorCiento) {
    const DiskUsage uso = crearUsoSimulado(0, 0);
    EXPECT_DOUBLE_EQ(0.0, porcentajeUsado(uso));
    EXPECT_DOUBLE_EQ(0.0, porcentajeLibre(uso));
}

// ---------------------------------------------------------------------------
// Puntos de montaje simulados: descarte de rutas virtuales
// ---------------------------------------------------------------------------

/// @brief Punto de montaje simulado, con su ruta y su tipo de sistema de archivos.
struct PuntoMontajeSimulado {
    std::string ruta;
    std::string tipo;
};

/// @brief Indica si un tipo de sistema de archivos es virtual y debe descartarse.
bool esSistemaDeArchivosVirtual(const std::string& tipo) {
    return tipo == "tmpfs" || tipo == "devtmpfs" || tipo == "proc" ||
           tipo == "sysfs" || tipo == "cgroup2" || tipo == "devpts";
}

/// @brief Devuelve solo los puntos de montaje que corresponden a discos reales.
std::vector<PuntoMontajeSimulado> descartarMontajesVirtuales(
    const std::vector<PuntoMontajeSimulado>& puntos) {
    std::vector<PuntoMontajeSimulado> reales;
    for (const PuntoMontajeSimulado& punto : puntos) {
        if (!esSistemaDeArchivosVirtual(punto.tipo)) {
            reales.push_back(punto);
        }
    }
    return reales;
}

/**
 * @brief Los puntos de montaje de tipo tmpfs se descartan.
 */
TEST(DiskUsageTest, DescartaPuntosDeMontajeTmpfs) {
    const std::vector<PuntoMontajeSimulado> puntos = {
        {"/", "ext4"},
        {"/dev/shm", "tmpfs"},
        {"/run", "tmpfs"},
        {"/home", "ext4"},
    };

    const std::vector<PuntoMontajeSimulado> reales = descartarMontajesVirtuales(puntos);

    ASSERT_EQ(static_cast<size_t>(2), reales.size());
    for (const PuntoMontajeSimulado& punto : reales) {
        EXPECT_NE("tmpfs", punto.tipo);
    }
}

/**
 * @brief Las particiones reales se conservan tras el descarte.
 */
TEST(DiskUsageTest, ConservaParticionesReales) {
    const std::vector<PuntoMontajeSimulado> puntos = {
        {"/", "ext4"},
        {"/run", "tmpfs"},
        {"/datos", "xfs"},
    };

    const std::vector<PuntoMontajeSimulado> reales = descartarMontajesVirtuales(puntos);

    ASSERT_EQ(static_cast<size_t>(2), reales.size());
    EXPECT_EQ("/", reales[0].ruta);
    EXPECT_EQ("/datos", reales[1].ruta);
}

/**
 * @brief Una lista compuesta solo por montajes virtuales queda vacía.
 */
TEST(DiskUsageTest, ListaSoloVirtualesQuedaVacia) {
    const std::vector<PuntoMontajeSimulado> puntos = {
        {"/proc", "proc"},
        {"/sys", "sysfs"},
        {"/dev", "devtmpfs"},
        {"/dev/shm", "tmpfs"},
    };

    EXPECT_TRUE(descartarMontajesVirtuales(puntos).empty());
}
} // namespace
