#include <gtest/gtest.h>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include "collectors/network/net_usage.cpp"

using chuspapulse::collectors::network::NetworkCollector;
using chuspapulse::collectors::ICollector;

class NetUsageTest : public ::testing::Test {
protected:
    std::filesystem::path directorio;
    std::filesystem::path ruta;

    void SetUp() override {
        std::string plantilla =
            (std::filesystem::temp_directory_path() /
             "chuspapulse_net_XXXXXX").string();

        const char* creado = mkdtemp(plantilla.data());
        ASSERT_NE(creado, nullptr);

        directorio = creado;
        ruta = directorio / "net_dev";
    }

    void TearDown() override {
        if (!directorio.empty()) {
            std::error_code error;
            std::filesystem::remove_all(directorio, error);
        }
    }

    void escribir(const std::string& contenido) {
        std::ofstream archivo(ruta);

        ASSERT_TRUE(archivo.is_open());

        archivo << "Inter-| Receive | Transmit\n";
        archivo << " face |bytes packets errs drop fifo frame "
                   "compressed multicast|bytes packets errs "
                   "drop fifo colls carrier compressed\n";
        archivo << contenido;

        archivo.close();
        ASSERT_FALSE(archivo.fail());
    }
};

TEST_F(NetUsageTest, LeeBytesDeUnaInterfaz) {
    escribir(
        "eth0: 1500 10 0 0 0 0 0 0 2500 20 0 0 0 0 0 0\n"
    );

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 1500U);
    EXPECT_EQ(uso.tx_bytes_total, 2500U);
}

TEST_F(NetUsageTest, SumaMultiplesInterfaces) {
    escribir(
        "eth0: 1000 10 0 0 0 0 0 0 2000 20 0 0 0 0 0 0\n"
        "wlan0: 3000 30 0 0 0 0 0 0 4000 40 0 0 0 0 0 0\n"
    );

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 4000U);
    EXPECT_EQ(uso.tx_bytes_total, 6000U);
}

TEST_F(NetUsageTest, ExcluyeInterfazLoopback) {
    escribir(
        "lo: 5000 50 0 0 0 0 0 0 5000 50 0 0 0 0 0 0\n"
        "eth0: 1000 10 0 0 0 0 0 0 2000 20 0 0 0 0 0 0\n"
    );

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 1000U);
    EXPECT_EQ(uso.tx_bytes_total, 2000U);
}

TEST_F(NetUsageTest, SoloLoopbackDevuelveCero) {
    escribir(
        "lo: 5000 50 0 0 0 0 0 0 5000 50 0 0 0 0 0 0\n"
    );

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 0U);
    EXPECT_EQ(uso.tx_bytes_total, 0U);
}

TEST_F(NetUsageTest, InterfazSinTrafico) {
    escribir(
        "eth0: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n"
    );

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 0U);
    EXPECT_EQ(uso.tx_bytes_total, 0U);
}

TEST_F(NetUsageTest, ArchivoSinInterfaces) {
    escribir("");

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 0U);
    EXPECT_EQ(uso.tx_bytes_total, 0U);
}

TEST_F(NetUsageTest, ToleraEspaciosIniciales) {
    escribir(
        "  eth0: 100 1 0 0 0 0 0 0 200 2 0 0 0 0 0 0\n"
        "\twlan0: 300 3 0 0 0 0 0 0 400 4 0 0 0 0 0 0\n"
    );

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 400U);
    EXPECT_EQ(uso.tx_bytes_total, 600U);
}

TEST_F(NetUsageTest, DistingueBytesDePaquetes) {
    escribir(
        "eth0: 8192 64 1 2 3 4 5 6 "
        "16384 128 7 8 9 10 11 12\n"
    );

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 8192U);
    EXPECT_EQ(uso.tx_bytes_total, 16384U);
}

TEST_F(NetUsageTest, ContadoresSuperioresA32Bits) {
    escribir(
        "eth0: 4294967296 10 0 0 0 0 0 0 "
        "8589934592 20 0 0 0 0 0 0\n"
    );

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 4294967296ULL);
    EXPECT_EQ(uso.tx_bytes_total, 8589934592ULL);
}

TEST_F(NetUsageTest, IncrementoDeBytesEntreLecturas) {
    escribir(
        "eth0: 1000 10 0 0 0 0 0 0 2000 20 0 0 0 0 0 0\n"
    );

    const auto inicial = getNetUsage(ruta.string());

    escribir(
        "eth0: 1600 16 0 0 0 0 0 0 2800 28 0 0 0 0 0 0\n"
    );

    const auto final = getNetUsage(ruta.string());

    ASSERT_GE(final.rx_bytes_total, inicial.rx_bytes_total);
    ASSERT_GE(final.tx_bytes_total, inicial.tx_bytes_total);

    EXPECT_EQ(final.rx_bytes_total - inicial.rx_bytes_total, 600U);
    EXPECT_EQ(final.tx_bytes_total - inicial.tx_bytes_total, 800U);
}

TEST_F(NetUsageTest, ArchivoInexistenteLanzaExcepcion) {
    EXPECT_THROW(
        getNetUsage(ruta.string()),
        std::runtime_error
    );
}

TEST_F(NetUsageTest, RutaVaciaLanzaExcepcion) {
    EXPECT_THROW(
        getNetUsage(""),
        std::runtime_error
    );
}

TEST_F(NetUsageTest, ErrorIncluyeRutaConsultada) {
    try {
        getNetUsage(ruta.string());
        FAIL() << "Se esperaba std::runtime_error";
    } catch (const std::runtime_error& error) {
        EXPECT_NE(
            std::string(error.what()).find(ruta.string()),
            std::string::npos
        );
    }
}

TEST_F(NetUsageTest, LecturaValidaDespuesDeUnError) {
    EXPECT_THROW(
        getNetUsage(ruta.string()),
        std::runtime_error
    );

    escribir(
        "eth0: 100 1 0 0 0 0 0 0 200 2 0 0 0 0 0 0\n"
    );

    const auto uso = getNetUsage(ruta.string());

    EXPECT_EQ(uso.rx_bytes_total, 100U);
    EXPECT_EQ(uso.tx_bytes_total, 200U);
}

TEST(NetworkCollectorTest, NombreEsNetwork) {
    NetworkCollector collector;

    EXPECT_EQ(collector.nombre(), "network");
}

TEST(NetworkCollectorTest, FuncionaComoICollector) {
    NetworkCollector collector;
    ICollector& interfaz = collector;

    EXPECT_EQ(interfaz.nombre(), "network");
    EXPECT_NO_THROW(interfaz.recolectar());
}

TEST(NetworkCollectorTest, DevuelveDosMetricas) {
    NetworkCollector collector;

    const auto metricas = collector.recolectar();

    EXPECT_EQ(metricas.size(), 2U);
}

TEST(NetworkCollectorTest, NombresYUnidadesCorrectos) {
    NetworkCollector collector;

    const auto metricas = collector.recolectar();

    ASSERT_EQ(metricas.size(), 2U);

    EXPECT_EQ(metricas[0].nombre, "net.rx");
    EXPECT_EQ(metricas[1].nombre, "net.tx");
    EXPECT_EQ(metricas[0].unidad, "bytes");
    EXPECT_EQ(metricas[1].unidad, "bytes");
}

TEST(NetworkCollectorTest, ValoresEnRangosValidos) {
    NetworkCollector collector;

    const auto metricas = collector.recolectar();

    ASSERT_EQ(metricas.size(), 2U);

    for (const auto& metrica : metricas) {
        EXPECT_TRUE(std::isfinite(metrica.valor));
        EXPECT_GE(metrica.valor, 0.0);
    }
}

TEST(NetworkCollectorTest, MarcasDeTiempoValidas) {
    NetworkCollector collector;

    const auto antes = static_cast<std::int64_t>(std::time(nullptr));
    const auto metricas = collector.recolectar();
    const auto despues = static_cast<std::int64_t>(std::time(nullptr));

    ASSERT_EQ(metricas.size(), 2U);

    for (const auto& metrica : metricas) {
        EXPECT_GE(metrica.timestamp, antes);
        EXPECT_LE(metrica.timestamp, despues);
    }

    EXPECT_EQ(metricas[0].timestamp, metricas[1].timestamp);
}
