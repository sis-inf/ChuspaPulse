#include <gtest/gtest.h>
#include "config/config.hpp"

namespace chuspapulse::config {

TEST(ConfigSamplerTest, DefaultValues) {
    Config config;

    ASSERT_EQ(config.sampler.intervalo_segundos, 10);
    ASSERT_EQ(config.servidor.host,              "0.0.0.0");
    ASSERT_EQ(config.servidor.puerto,            8080);
    ASSERT_EQ(config.storage.ruta_db,            "chuspapulse.db");
    ASSERT_EQ(config.nivel_log,                  "info");
}

TEST(UmbralesConfigTest, DefaultValues) {
    UmbralesConfig umbrales;

    EXPECT_DOUBLE_EQ(umbrales.cpu_max,   80.0);
    EXPECT_DOUBLE_EQ(umbrales.ram_max,   85.0);
    EXPECT_DOUBLE_EQ(umbrales.disco_max, 90.0);
}

TEST(MonitorConfigTest, DefaultValues) {
    MonitorConfig monitor;

    EXPECT_EQ(monitor.interval_ms, 1000);
    EXPECT_TRUE(monitor.cpu);
    EXPECT_TRUE(monitor.ram);
    EXPECT_TRUE(monitor.disk);
}

TEST(ErrorConfigTest, WhatPreservesMessage) {
    ErrorConfig error("error de prueba");

    EXPECT_STREQ(error.what(), "error de prueba");
}

} // namespace chuspapulse::config