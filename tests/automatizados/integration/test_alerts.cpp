#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <string>
#include <thread>
#include <vector>

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

TEST_F(AlertsEndpointTest, RespuestaEsJsonValido) {
    snapshot_ = chuspapulse::MetricSnapshot(95.0, 10.0, 10.0, 1.0, 1.0);  // cpu sobre el umbral

    httplib::Client cli("127.0.0.1", port_);
    auto res = cli.Get("/alerts");

    ASSERT_TRUE(res);

    nlohmann::json json;
    ASSERT_NO_THROW(json = nlohmann::json::parse(res->body));
    ASSERT_EQ(json["alerts"].size(), 1U);
    EXPECT_EQ(json["alerts"][0]["id"], 1);
    EXPECT_EQ(json["alerts"][0]["message"].get<std::string>().rfind("Alerta: cpu >", 0), 0U);
}

TEST(SerializarAlertasTest, ListaVaciaEsJsonValido) {
    const std::string salida = chuspapulse::http::serializarAlertas({});

    nlohmann::json json;
    ASSERT_NO_THROW(json = nlohmann::json::parse(salida));
    EXPECT_TRUE(json["alerts"].is_array());
    EXPECT_TRUE(json["alerts"].empty());
}

TEST(SerializarAlertasTest, CaracteresEspecialesSeEscapan) {
    const std::string mensaje =
        "dice \"hola\" en C:\\ruta\nsegunda linea\tcon tab y control \x01";

    const std::string salida = chuspapulse::http::serializarAlertas(
        {{7, mensaje, "2026-01-01T00:00:00Z"}});

    EXPECT_NE(salida.find("\\\"hola\\\""), std::string::npos);
    EXPECT_NE(salida.find("C:\\\\ruta"), std::string::npos);
    EXPECT_NE(salida.find("\\nsegunda linea"), std::string::npos);
    EXPECT_NE(salida.find("\\tcon tab"), std::string::npos);
    EXPECT_NE(salida.find("\\u0001"), std::string::npos);

    nlohmann::json json;
    ASSERT_NO_THROW(json = nlohmann::json::parse(salida));
    ASSERT_EQ(json["alerts"].size(), 1U);
    EXPECT_EQ(json["alerts"][0]["id"], 7);
    EXPECT_EQ(json["alerts"][0]["message"], mensaje);
    EXPECT_EQ(json["alerts"][0]["timestamp"], "2026-01-01T00:00:00Z");
}

TEST(SerializarAlertasTest, VariasAlertasConservanOrden) {
    const std::vector<chuspapulse::http::AlertaActiva> alertas = {
        {1, "primera \"a\"", "2026-01-01T00:00:00Z"},
        {3, "tercera\\b", "2026-01-01T00:00:00Z"},
    };

    const nlohmann::json json =
        nlohmann::json::parse(chuspapulse::http::serializarAlertas(alertas));

    ASSERT_EQ(json["alerts"].size(), 2U);
    EXPECT_EQ(json["alerts"][0]["id"], 1);
    EXPECT_EQ(json["alerts"][0]["message"], "primera \"a\"");
    EXPECT_EQ(json["alerts"][1]["id"], 3);
    EXPECT_EQ(json["alerts"][1]["message"], "tercera\\b");
}