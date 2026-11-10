#include <gtest/gtest.h>

#include <string>

#include "core/types.hpp"
#include "formatters/formatter_json.hpp"

namespace {

chuspapulse::core::Snapshot crearSnapshotConocido()
{
    chuspapulse::core::Snapshot snapshot;

    snapshot.timestamp = 1710000000;

    snapshot.metricas = {
        {"cpu", 42.5, "porcentaje", 1710000000},
        {"memory", 68.0, "porcentaje", 1710000000},
        {"disk", 71.3, "porcentaje", 1710000000},
        {"network", 2048.0, "bytes", 1710000000}
    };

    return snapshot;
}

bool llavesBalanceadas(const std::string& texto)
{
    int balance = 0;

    for (char c : texto) {
        if (c == '{') {
            ++balance;
        } else if (c == '}') {
            --balance;

            if (balance < 0) {
                return false;
            }
        }
    }

    return balance == 0;
}

} // namespace

TEST(FormatterJSONTest, ResultadoNoEsVacio)
{
    chuspapulse::core::Snapshot snapshot = crearSnapshotConocido();
    chuspapulse::formatters::FormatterJSON formatter;

    const std::string resultado = formatter.formatear(snapshot);

    EXPECT_FALSE(resultado.empty());
}

TEST(FormatterJSONTest, ContieneCpu)
{
    chuspapulse::core::Snapshot snapshot = crearSnapshotConocido();
    chuspapulse::formatters::FormatterJSON formatter;

    const std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("cpu"), std::string::npos);
}

TEST(FormatterJSONTest, ContieneTimestamp)
{
    chuspapulse::core::Snapshot snapshot = crearSnapshotConocido();
    chuspapulse::formatters::FormatterJSON formatter;

    const std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("timestamp"), std::string::npos);
}

TEST(FormatterJSONTest, ContieneCamposRequeridos)
{
    chuspapulse::core::Snapshot snapshot = crearSnapshotConocido();
    chuspapulse::formatters::FormatterJSON formatter;

    const std::string resultado = formatter.formatear(snapshot);

    EXPECT_NE(resultado.find("cpu"), std::string::npos);
    EXPECT_NE(resultado.find("memory"), std::string::npos);
    EXPECT_NE(resultado.find("disk"), std::string::npos);
    EXPECT_NE(resultado.find("network"), std::string::npos);
    EXPECT_NE(resultado.find("timestamp"), std::string::npos);
}

TEST(FormatterJSONTest, LlavesEstanBalanceadas)
{
    chuspapulse::core::Snapshot snapshot = crearSnapshotConocido();
    chuspapulse::formatters::FormatterJSON formatter;

    const std::string resultado = formatter.formatear(snapshot);

    EXPECT_TRUE(llavesBalanceadas(resultado));
}