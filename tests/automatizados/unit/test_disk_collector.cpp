#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <ctime>

#include "collectors/disk/disk_usage.cpp"
#include "collectors/icollector.hpp"

using chuspapulse::collectors::ICollector;
using chuspapulse::collectors::disk::DiskCollector;

TEST(DiskCollectorTest, NombreEsDisk) {
    DiskCollector collector;

    EXPECT_EQ(collector.nombre(), "disk");
}

TEST(DiskCollectorTest, FuncionaComoICollector) {
    DiskCollector collector;
    ICollector& base = collector;

    EXPECT_EQ(base.nombre(), "disk");
    EXPECT_NO_THROW(base.recolectar());
}

TEST(DiskCollectorTest, RecoleccionNoLanzaExcepcion) {
    DiskCollector collector;

    EXPECT_NO_THROW(collector.recolectar());
}

TEST(DiskCollectorTest, DevuelveTresMetricas) {
    DiskCollector collector;

    const auto metricas = collector.recolectar();

    EXPECT_EQ(metricas.size(), 3u);
}

TEST(DiskCollectorTest, NombresDeMetricasCorrectos) {
    DiskCollector collector;

    const auto metricas = collector.recolectar();

    ASSERT_EQ(metricas.size(), 3u);

    EXPECT_EQ(metricas[0].nombre, "disk.total");
    EXPECT_EQ(metricas[1].nombre, "disk.used");
    EXPECT_EQ(metricas[2].nombre, "disk.free");
}

TEST(DiskCollectorTest, UnidadesExpresadasEnBytes) {
    DiskCollector collector;

    const auto metricas = collector.recolectar();

    ASSERT_EQ(metricas.size(), 3u);

    for (const auto& metrica : metricas) {
        EXPECT_EQ(metrica.unidad, "bytes");
    }
}

TEST(DiskCollectorTest, ValoresNumericosFinitos) {
    DiskCollector collector;

    const auto metricas = collector.recolectar();

    ASSERT_EQ(metricas.size(), 3u);

    for (const auto& metrica : metricas) {
        EXPECT_TRUE(std::isfinite(metrica.valor));
    }
}

TEST(DiskCollectorTest, EspacioEnRangosValidos) {
    DiskCollector collector;

    const auto metricas = collector.recolectar();

    ASSERT_EQ(metricas.size(), 3u);

    const double total = metricas[0].valor;
    const double usado = metricas[1].valor;
    const double libre = metricas[2].valor;

    EXPECT_GT(total, 0.0);

    EXPECT_GE(usado, 0.0);
    EXPECT_LE(usado, total);

    EXPECT_GE(libre, 0.0);
    EXPECT_LE(libre, total);
}

TEST(DiskCollectorTest, CalculoDeEspacioEsCoherente) {
    DiskCollector collector;

    const auto metricas = collector.recolectar();

    ASSERT_EQ(metricas.size(), 3u);

    const double total = metricas[0].valor;
    const double usado = metricas[1].valor;
    const double libre = metricas[2].valor;

    EXPECT_DOUBLE_EQ(usado, total - libre);
    EXPECT_DOUBLE_EQ(total, usado + libre);
}

TEST(DiskCollectorTest, MarcasDeTiempoValidas) {
    DiskCollector collector;

    const auto antes =
        static_cast<std::int64_t>(std::time(nullptr));

    const auto metricas = collector.recolectar();

    const auto despues =
        static_cast<std::int64_t>(std::time(nullptr));

    ASSERT_EQ(metricas.size(), 3u);

    for (const auto& metrica : metricas) {
        EXPECT_GT(metrica.timestamp, 0);
        EXPECT_GE(metrica.timestamp, antes);
        EXPECT_LE(metrica.timestamp, despues);
        EXPECT_EQ(metrica.timestamp, metricas[0].timestamp);
    }
}

TEST(DiskCollectorTest, LecturasConsecutivasSonCoherentes) {
    DiskCollector collector;

    const auto primera = collector.recolectar();
    const auto segunda = collector.recolectar();

    ASSERT_EQ(primera.size(), 3u);
    ASSERT_EQ(segunda.size(), 3u);

    for (const auto& metricas : {primera, segunda}) {
        EXPECT_GT(metricas[0].valor, 0.0);
        EXPECT_GE(metricas[1].valor, 0.0);
        EXPECT_GE(metricas[2].valor, 0.0);

        EXPECT_DOUBLE_EQ(
            metricas[0].valor,
            metricas[1].valor + metricas[2].valor
        );
    }
}

TEST(DiskCollectorTest, RutaVaciaLanzaErrorRecoleccion) {
    EXPECT_THROW(
        getDiskUsage(""),
        chuspapulse::collectors::ErrorRecoleccion
    );
}

TEST(DiskCollectorTest, RutaInexistenteLanzaErrorRecoleccion) {
    EXPECT_THROW(
        getDiskUsage("/dev/null/chuspapulse_disk"),
        chuspapulse::collectors::ErrorRecoleccion
    );
}

TEST(DiskCollectorTest, ErrorIncluyeRutaConsultada) {
    const std::string ruta = "/dev/null/chuspapulse_disk";

    try {
        getDiskUsage(ruta);
        FAIL() << "Se esperaba ErrorRecoleccion";
    } catch (const chuspapulse::collectors::ErrorRecoleccion& error) {
        const std::string mensaje = error.what();

        EXPECT_NE(mensaje.find(ruta), std::string::npos);
    }
}

TEST(DiskCollectorTest, ErrorIncluyeCodigoErrno) {
    try {
        getDiskUsage("");
        FAIL() << "Se esperaba ErrorRecoleccion";
    } catch (const chuspapulse::collectors::ErrorRecoleccion& error) {
        const std::string mensaje = error.what();

        EXPECT_NE(
            mensaje.find("errno: " + std::to_string(ENOENT)),
            std::string::npos
        );
    }
}

TEST(DiskCollectorTest, LecturaValidaDespuesDeUnError) {
    EXPECT_THROW(
        getDiskUsage(""),
        chuspapulse::collectors::ErrorRecoleccion
    );

    DiskCollector collector;

    EXPECT_NO_THROW(collector.recolectar());
}

TEST(DiskCollectorTest, EspacioTotalCoincideConStatvfs) {
    struct statvfs datos{};

    ASSERT_EQ(statvfs("/", &datos), 0);

    const auto totalEsperado =
        static_cast<std::uint64_t>(datos.f_blocks) *
        datos.f_frsize;

    const auto uso = getDiskUsage("/");

    EXPECT_EQ(uso.total, totalEsperado);
    EXPECT_LE(uso.libre, uso.total);
    EXPECT_LE(uso.usado, uso.total);
    EXPECT_EQ(uso.usado, uso.total - uso.libre);
}
