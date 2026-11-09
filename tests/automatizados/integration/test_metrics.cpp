#include <gtest/gtest.h>
#include <httplib.h>

#include <ctime>
#include <filesystem>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <thread>

#include "http/handler_metrics.hpp"
#include "http/handler_prometheus.hpp"
#include "storage/storage.hpp"

namespace fs = std::filesystem;

// Levanta /metrics y /metrics/prometheus sobre un Storage temporal.
class IntegrationMetricsTest : public ::testing::Test {
protected:
  std::string ruta_db_;
  std::unique_ptr<chuspapulse::storage::Storage> storage_;
  httplib::Server servidor_;
  std::thread hilo_;
  int puerto_ = 0;

  void SetUp() override {
    ruta_db_ = (fs::temp_directory_path() /
                (std::string(::testing::UnitTest::GetInstance()
                                 ->current_test_info()
                                 ->name()) +
                 ".db"))
                   .string();
    fs::remove(ruta_db_);
    storage_ = std::make_unique<chuspapulse::storage::Storage>(ruta_db_);

    chuspapulse::http::HandleMetrics(servidor_, *storage_);
    chuspapulse::http::registrarPrometheus(servidor_, *storage_);

    puerto_ = servidor_.bind_to_any_port("127.0.0.1");
    ASSERT_GT(puerto_, 0);
    hilo_ = std::thread([this]() { servidor_.listen_after_bind(); });
    servidor_.wait_until_ready();
  }

  void TearDown() override {
    servidor_.stop();
    if (hilo_.joinable()) {
      hilo_.join();
    }
    storage_.reset();
    fs::remove(ruta_db_);
  }

  void guardarSnapshot(double cpu, double ram_usada) {
    const std::int64_t ahora = static_cast<std::int64_t>(std::time(nullptr));

    chuspapulse::core::Snapshot snapshot;
    snapshot.timestamp = ahora;
    snapshot.metricas = {
        {"cpu.usage", cpu, "porcentaje", ahora},
        {"memory.used", ram_usada, "bytes", ahora},
    };
    storage_->save(snapshot);
  }

  httplib::Result get(const std::string &ruta) {
    httplib::Client cliente("127.0.0.1", puerto_);
    return cliente.Get(ruta);
  }
};

TEST_F(IntegrationMetricsTest, SinDatosDevuelve503) {
  auto res = get("/metrics");

  ASSERT_TRUE(res);
  EXPECT_EQ(res->status, 503);
  EXPECT_EQ(res->body, R"({"error":"no data available"})");
}

TEST_F(IntegrationMetricsTest, ConDatosDevuelveMetricasEnFormatoPrometheus) {
  guardarSnapshot(21.5, 1024);

  auto res = get("/metrics");

  ASSERT_TRUE(res);
  EXPECT_EQ(res->status, 200);
  EXPECT_EQ(res->get_header_value("Content-Type"), "text/plain; version=0.0.4");
  EXPECT_NE(res->body.find("# HELP chuspapulse_cpu_usage "), std::string::npos);
  EXPECT_NE(res->body.find("# TYPE chuspapulse_cpu_usage gauge\n"),
            std::string::npos);
  EXPECT_NE(res->body.find("\nchuspapulse_cpu_usage 21.5\n"), std::string::npos);
  EXPECT_NE(res->body.find("\nchuspapulse_memory_used 1024\n"), std::string::npos);
}

TEST_F(IntegrationMetricsTest, NombresYLineasSonValidosParaPrometheus) {
  guardarSnapshot(21.5, 1024);

  auto res = get("/metrics");
  ASSERT_TRUE(res);
  ASSERT_EQ(res->status, 200);

  const std::regex comentario(
      R"(^# (HELP|TYPE) [a-zA-Z_:][a-zA-Z0-9_:]* .+$)");
  const std::regex muestra(
      R"(^[a-zA-Z_:][a-zA-Z0-9_:]*(\{[^}]*\})? -?[0-9]+(\.[0-9]+)?([eE][-+]?[0-9]+)?$)");

  std::istringstream lineas(res->body);
  std::string linea;
  int muestras = 0;
  while (std::getline(lineas, linea)) {
    if (linea.empty()) {
      continue;
    }
    if (linea[0] == '#') {
      EXPECT_TRUE(std::regex_match(linea, comentario)) << linea;
    } else {
      EXPECT_TRUE(std::regex_match(linea, muestra)) << linea;
      ++muestras;
    }
  }
  EXPECT_GT(muestras, 0);
}

TEST_F(IntegrationMetricsTest, ExponeElSnapshotMasReciente) {
  guardarSnapshot(10.0, 1024);
  guardarSnapshot(55.25, 2048);

  auto res = get("/metrics");

  ASSERT_TRUE(res);
  EXPECT_EQ(res->status, 200);
  EXPECT_NE(res->body.find("\nchuspapulse_cpu_usage 55.25\n"), std::string::npos);
  EXPECT_EQ(res->body.find("chuspapulse_cpu_usage 10\n"), std::string::npos);
}

TEST_F(IntegrationMetricsTest, CoincideConMetricsPrometheus) {
  guardarSnapshot(21.5, 1024);

  auto metrics = get("/metrics");
  auto prometheus = get("/metrics/prometheus");

  ASSERT_TRUE(metrics);
  ASSERT_TRUE(prometheus);
  EXPECT_EQ(metrics->body, prometheus->body);
}
