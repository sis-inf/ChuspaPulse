#include <gtest/gtest.h>
#include <httplib.h>

// Test de integración para validar el endpoint /config
TEST(IntegrationConfigTest, GetConfigReturns200) {
    // Nota: Asumiendo que el entorno de pruebas levanta el servidor o se prueba de forma local
    httplib::Client cli("localhost", 8080); // O el puerto configurado en las pruebas
    
    auto res = cli.Get("/config");
    
    ASSERT_TRUE(res != nullptr);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(res->get_header_value("Content-Type"), "application/json");
    
    // Validar que la respuesta contenga los campos requeridos (interval_ms, http_port, log_level, etc.)
    EXPECT_NE(res->body.find("interval_ms"), std::string::npos);
    EXPECT_NE(res->body.find("http_port"), std::string::npos);
    EXPECT_NE(res->body.find("log_level"), std::string::npos);
}