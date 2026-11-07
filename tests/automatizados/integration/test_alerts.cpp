#include <gtest/gtest.h>
#include <httplib.h>

#include <string>
#include <thread>

#include "alertas/alerta_umbral.hpp"
#include "alertas/gestor_alertas.hpp"
#include "core/MetricSnapshot.hpp"
#include "http/handler_alerts.hpp"

class AlertsEndpointTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Regla de prueba: alerta si cpu > 80
        gestor_.addAlerta(chuspapulse::alertas::AlertaUmbral("cpu", 80.0f, '>'));

        server_.Get("/alerts", [this](const httplib::Request&, httplib::Response& res) {
            res.status = 200;
            res.set_content(chuspapulse::http::handleAlerts(gestor_, snapshot_),
                            "application/json");
        });

        port_ = server_.bind_to_any_port("127.0.0.1");
        thread_ = std::thread([this] { server_.listen_after_bind(); });
        server_.wait_until_ready();
    }

    void TearDown() override {
        server_.stop();
        if (thread_.joinable()) thread_.join();
    }

    chuspapulse::alertas::GestorAlertas gestor_;
    chuspapulse::MetricSnapshot snapshot_;
    httplib::Server server_;
    std::thread thread_;
    int port_ = 0;
};

TEST_F(AlertsEndpointTest, SinAlertasActivasDevuelveListaVacia) {
    snapshot_ = chuspapulse::MetricSnapshot(10.0, 10.0, 10.0, 1.0, 1.0);  // cpu bajo el umbral

    httplib::Client cli("127.0.0.1", port_);
    auto res = cli.Get("/alerts");

    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_NE(res->body.find("\"alerts\": []"), std::string::npos);
}

TEST_F(AlertsEndpointTest, ConAlertasActivasIncluyeLaAlerta) {
    snapshot_ = chuspapulse::MetricSnapshot(95.0, 10.0, 10.0, 1.0, 1.0);  // cpu sobre el umbral

    httplib::Client cli("127.0.0.1", port_);
    auto res = cli.Get("/alerts");

    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_NE(res->body.find("Alerta: cpu >"), std::string::npos);
    EXPECT_EQ(res->body.find("\"alerts\": []"), std::string::npos);
}