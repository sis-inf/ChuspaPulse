#include <gtest/gtest.h>
#include <httplib.h>

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "http/handler_health.hpp"

using chuspapulse::http::ComprobacionReadiness;

// Levanta /health y /ready en un puerto libre con comprobaciones controladas.
class IntegrationHealthTest : public ::testing::Test {
protected:
  httplib::Server servidor_;
  std::thread hilo_;
  int puerto_ = 0;
  std::vector<ComprobacionReadiness> comprobaciones_;

  void iniciar(std::vector<ComprobacionReadiness> comprobaciones) {
    comprobaciones_ = std::move(comprobaciones);
    const auto inicio = std::chrono::steady_clock::now();

    servidor_.Get("/health",
                  [inicio](const httplib::Request &, httplib::Response &res) {
                    res.set_content(chuspapulse::http::handleHealth(inicio),
                                    "application/json");
                  });

    servidor_.Get("/ready",
                  [this](const httplib::Request &, httplib::Response &res) {
                    const auto resultado =
                        chuspapulse::http::handleReadiness(comprobaciones_);
                    res.status = resultado.status;
                    res.set_content(resultado.cuerpo, "application/json");
                  });

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
  }

  httplib::Result get(const std::string &ruta) {
    httplib::Client cliente("127.0.0.1", puerto_);
    return cliente.Get(ruta);
  }
};

TEST_F(IntegrationHealthTest, ReadyConComponentesDisponiblesDevuelve200) {
  iniciar({
      {"storage", []() { return true; }},
      {"sampler", []() { return true; }},
  });

  auto res = get("/ready");

  ASSERT_TRUE(res);
  EXPECT_EQ(res->status, 200);
  EXPECT_EQ(res->get_header_value("Content-Type"), "application/json");
  EXPECT_EQ(res->body,
            R"({"status":"ready","componentes":{"storage":true,"sampler":true}})");
}

TEST_F(IntegrationHealthTest, ReadyConComponenteNoDisponibleDevuelve503) {
  iniciar({
      {"storage", []() { return true; }},
      {"sampler", []() { return false; }},
  });

  auto res = get("/ready");

  ASSERT_TRUE(res);
  EXPECT_EQ(res->status, 503);
  EXPECT_EQ(
      res->body,
      R"({"status":"not_ready","componentes":{"storage":true,"sampler":false}})");
}

TEST_F(IntegrationHealthTest, ReadyConComprobacionQueLanzaDevuelve503) {
  iniciar({
      {"storage", []() -> bool { throw std::runtime_error("db no disponible"); }},
      {"sampler", []() { return true; }},
  });

  auto res = get("/ready");

  ASSERT_TRUE(res);
  EXPECT_EQ(res->status, 503);
  EXPECT_EQ(
      res->body,
      R"({"status":"not_ready","componentes":{"storage":false,"sampler":true}})");
}

TEST_F(IntegrationHealthTest, HealthResponde200AunqueReadinessFalle) {
  iniciar({
      {"storage", []() { return false; }},
  });

  auto health = get("/health");
  auto ready = get("/ready");

  ASSERT_TRUE(health);
  ASSERT_TRUE(ready);
  EXPECT_EQ(health->status, 200);
  EXPECT_NE(health->body.find(R"("status":"ok")"), std::string::npos);
  EXPECT_EQ(ready->status, 503);
}
