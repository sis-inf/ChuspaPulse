#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <atomic>

class HttpIntegrationTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        server_thread = std::thread([]() {
            httplib::Server svr;
            svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
                res.set_content("{\"status\":\"ok\"}", "application/json");
            });
            svr.Get("/metrics", [](const httplib::Request&, httplib::Response& res) {
                res.set_content("{}", "application/json");
            });
            svr.Get("/metrics/history", [](const httplib::Request&, httplib::Response& res) {
                res.set_content("[]", "application/json");
            });
            svr.Get("/config", [](const httplib::Request&, httplib::Response& res) {
                res.set_content("{\"http_port\":8080}", "application/json");
            });
            svr.Get("/alerts", [](const httplib::Request&, httplib::Response& res) {
                res.set_content("[]", "application/json");
            });
            svr.Get("/version", [](const httplib::Request&, httplib::Response& res) {
                res.set_content("{\"version\":\"1.0.0\"}", "application/json");
            });
            svr.Get("/metrics/prometheus", [](const httplib::Request&, httplib::Response& res) {
                res.set_content("metric_name 1.0", "text/plain");
            });
            server_ptr = &svr;
            svr.listen("localhost", 8080);
        });
        // Wait for server to start
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    static void TearDownTestSuite() {
        if (server_ptr) {
            server_ptr->stop();
        }
        if (server_thread.joinable()) {
            server_thread.join();
        }
    }

    static std::thread server_thread;
    static httplib::Server* server_ptr;
};

std::thread HttpIntegrationTest::server_thread;
httplib::Server* HttpIntegrationTest::server_ptr = nullptr;

// Helper para validar JSON
void VerifyEndpoint(const std::string& path, const std::string& expected_structure_field = "") {
    httplib::Client cli("localhost", 8080);
    auto res = cli.Get(path.c_str());

    ASSERT_TRUE(res != nullptr) << "No hubo respuesta del servidor en: " << path;
    EXPECT_EQ(res->status, 200) << "Error en el endpoint: " << path << ". Código: " << res->status;
    
    if (res->status == 200) {
        try {
            nlohmann::json j = nlohmann::json::parse(res->body);
            if (!expected_structure_field.empty()) {
                EXPECT_TRUE(j.contains(expected_structure_field)) << "JSON en " << path << " no contiene campo: " << expected_structure_field;
            }
        } catch (const nlohmann::json::parse_error& e) {
            // No fallar si no es JSON (como en metrics/prometheus)
            if (path != "/metrics/prometheus") {
                FAIL() << "Error al parsear JSON en " << path << ": " << e.what();
            }
        }
    }
}

TEST_F(HttpIntegrationTest, HealthEndpoint) {
    VerifyEndpoint("/health", "status");
}

TEST_F(HttpIntegrationTest, MetricsEndpoint) {
    VerifyEndpoint("/metrics"); 
}

TEST_F(HttpIntegrationTest, MetricsHistoryEndpoint) {
    VerifyEndpoint("/metrics/history");
}

TEST_F(HttpIntegrationTest, ConfigEndpoint) {
    VerifyEndpoint("/config", "http_port");
}

TEST_F(HttpIntegrationTest, AlertsEndpoint) {
    VerifyEndpoint("/alerts");
}

TEST_F(HttpIntegrationTest, VersionEndpoint) {
    VerifyEndpoint("/version", "version");
}

TEST_F(HttpIntegrationTest, MetricsPrometheusEndpoint) {
    httplib::Client cli("localhost", 8080);
    auto res = cli.Get("/metrics/prometheus");
    ASSERT_TRUE(res != nullptr);
    EXPECT_EQ(res->status, 200);
}
