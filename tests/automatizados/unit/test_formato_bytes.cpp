
#include <gtest/gtest.h>

#include "utils/formato_bytes.hpp"

TEST(FormatoBytesHumanosTest, BytesMenoresA1024) {
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(800), "800.0 B");
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(1), "1.0 B");
}

TEST(FormatoBytesHumanosTest, PotenciasDe1024) {
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(1024), "1.0 KB");
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(1048576), "1.0 MB");
}

TEST(FormatoBytesHumanosTest, ValoresIntermedios) {
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(1536), "1.5 KB");
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(2621440), "2.5 MB");
}

TEST(FormatoBytesHumanosTest, ValoresNegativos) {
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(-1536), "-1.5 KB");
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(-800), "-800.0 B");
}

TEST(FormatoBytesHumanosTest, ValorCero) {
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(0), "0.0 B");
}

TEST(FormatoBytesHumanosTest, GigabyteExacto) {
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(1073741824), "1.0 GB");
}

TEST(FormatoBytesHumanosTest, TerabyteExacto) {
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(1099511627776.0), "1.0 TB");
}

TEST(FormatoBytesHumanosTest, TopeEnTerabytes) {
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(1024.0 * 1099511627776.0), "1024.0 TB");
}

TEST(FormatoBytesHumanosTest, RedondeoAUnDecimal) {
    EXPECT_EQ(chuspapulse::utils::formatearBytesHumanos(1.5), "1.5 B");
}
