#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <chrono>
#include <string>
#include "http/handler_metrics.hpp"

class MetricsTest : public ::testing::Test {
protected:
    std::string dbPath = "test_metrics.db";
    pulso::storage::Storage* storage = nullptr;
    httplib::Server server;
    std::thread serverThread;
    int port = 18080;

    static pulso::core::Metrica crearMetrica(
        const std::string& nombre,
        double valor,
        const std::string& unidad,
        std::int64_t timestamp)
    {
        pulso::core::Metrica metrica;
        metrica.nombre = nombre;
        metrica.valor = valor;
        metrica.unidad = unidad;
        metrica.timestamp = timestamp;
        return metrica;
    }

    void SetUp() override {
        std::remove(dbPath.c_str());
        storage = new pulso::storage::Storage(dbPath);

        // Snapshot anterior: sus metricas no deben aparecer en /metrics
        pulso::core::Snapshot anterior;
        anterior.timestamp = 1234567800;
        anterior.metricas.push_back(crearMetrica("net.anterior", 999.0, "bytes", anterior.timestamp));
        anterior.metricas.push_back(crearMetrica("cpu.anterior", 10.0, "percent", anterior.timestamp));
        storage->save(anterior);

        // Snapshot mas reciente: tres metricas
        pulso::core::Snapshot snapshot;
        snapshot.timestamp = 1234567890;
        snapshot.metricas.push_back(crearMetrica("cpu.usage", 45.5, "percent", snapshot.timestamp));
        snapshot.metricas.push_back(crearMetrica("ram.used", 2048.0, "bytes", snapshot.timestamp));
        snapshot.metricas.push_back(crearMetrica("disk.used", 73.25, "percent", snapshot.timestamp));
        storage->save(snapshot);

        pulso::http::HandleMetrics(server, *storage);

        serverThread = std::thread([this]() {
            server.listen("127.0.0.1", port);
        });

        for (int i = 0; i < 50 && !server.is_running(); ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    void TearDown() override {
        server.stop();

        if (serverThread.joinable()) {
            serverThread.join();
        }

        delete storage;
        storage = nullptr;

        std::remove(dbPath.c_str());
        std::remove((dbPath + "-wal").c_str());
        std::remove((dbPath + "-shm").c_str());
    }
};

TEST_F(MetricsTest, GetMetricsReturns200) {
    httplib::Client client("127.0.0.1", port);
    auto response = client.Get("/metrics");

    ASSERT_NE(response, nullptr);
    EXPECT_EQ(response->status, 200);
    EXPECT_EQ(response->get_header_value("Content-Type"), "application/json");
    EXPECT_NE(response->body.find("timestamp"), std::string::npos);
    EXPECT_NE(response->body.find("metricas"), std::string::npos);
    EXPECT_NE(response->body.find("cpu.usage"), std::string::npos);
    EXPECT_NE(response->body.find("45.5"), std::string::npos);
}

TEST_F(MetricsTest, GetMetricsReturnsAllMetricsOfLastSnapshot) {
    httplib::Client client("127.0.0.1", port);
    auto response = client.Get("/metrics");

    ASSERT_NE(response, nullptr);
    ASSERT_EQ(response->status, 200);

    const std::string& body = response->body;

    // Las tres metricas del ultimo snapshot
    EXPECT_NE(body.find("cpu.usage"), std::string::npos);
    EXPECT_NE(body.find("ram.used"), std::string::npos);
    EXPECT_NE(body.find("disk.used"), std::string::npos);
    EXPECT_NE(body.find("45.5"), std::string::npos);
    EXPECT_NE(body.find("2048"), std::string::npos);
    EXPECT_NE(body.find("73.25"), std::string::npos);
    EXPECT_NE(body.find("1234567890"), std::string::npos);

    // Nada del snapshot anterior
    EXPECT_EQ(body.find("net.anterior"), std::string::npos);
    EXPECT_EQ(body.find("cpu.anterior"), std::string::npos);
    EXPECT_EQ(body.find("1234567800"), std::string::npos);
}
