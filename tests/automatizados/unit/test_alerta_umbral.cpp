#include <gtest/gtest.h>
#include <stdexcept>
#include "alertas/alerta_umbral.hpp"
#include "core/MetricSnapshot.hpp"

using namespace chuspapulse::alertas;
using namespace chuspapulse;

TEST(AlertaUmbralTest, OperadorMayorDisparaCuandoSuperaUmbral)
{
    AlertaUmbral alerta("cpu.usage_pct", 50.0f, '>');
    
    MetricSnapshot snap;
    snap.cpu = 65.5;
    
    EXPECT_TRUE(alerta.evaluar(snap));
}

TEST(AlertaUmbralTest, OperadorMayorNoDisparaSiEstaPorDebajo)
{
    AlertaUmbral alerta("cpu.usage_pct", 50.0f, '>');
    
    MetricSnapshot snap;
    snap.cpu = 30.0;
    
    EXPECT_FALSE(alerta.evaluar(snap));
}

TEST(AlertaUmbralTest, OperadorMenorDisparaCuandoEsInferiorAlUmbral)
{
    AlertaUmbral alerta("ram", 20.0f, '<');
    
    MetricSnapshot snap;
    snap.ram = 15.2;
    
    EXPECT_TRUE(alerta.evaluar(snap));
}

TEST(AlertaUmbralTest, LanzaInvalidArgumentSiElOperadorEsInvalido)
{
    EXPECT_THROW(
        AlertaUmbral("cpu.usage_pct", 50.0f, 'X'),
        std::invalid_argument
    );
}

TEST(AlertaUmbralTest, MensajeRetornaStringNoVacio)
{
    AlertaUmbral alerta("cpu", 38.0f, '>');

    std::string msg = alerta.mensaje();
    EXPECT_FALSE(msg.empty());
}

TEST(AlertaUmbralTest, ConstructorLanzaInvalidArgumentSiLaMetricaNoExiste)
{
    EXPECT_THROW(
        AlertaUmbral("temperatura", 38.0f, '>'),
        std::invalid_argument
    );
}

TEST(AlertaUmbralTest, ErrorDeMetricaIdentificaLaMetricaInvalida)
{
    try {
        AlertaUmbral("temperatura", 38.0f, '>');
        FAIL() << "Se esperaba std::invalid_argument";
    } catch (const std::invalid_argument& e) {
        EXPECT_NE(std::string(e.what()).find("'temperatura'"), std::string::npos);
    }
}

TEST(AlertaUmbralTest, ConstructorLanzaInvalidArgumentSiLaMetricaEstaVacia)
{
    EXPECT_THROW(
        AlertaUmbral("", 50.0f, '>'),
        std::invalid_argument
    );
}

TEST(AlertaUmbralTest, ErrorDeOperadorIdentificaElOperadorInvalido)
{
    try {
        AlertaUmbral("cpu", 50.0f, '=');
        FAIL() << "Se esperaba std::invalid_argument";
    } catch (const std::invalid_argument& e) {
        const std::string msg = e.what();
        EXPECT_NE(msg.find("Operador no válido"), std::string::npos);
        EXPECT_NE(msg.find("'='"), std::string::npos);
    }
}

TEST(AlertaUmbralTest, ConstructorAceptaTodasLasMetricasValidas)
{
    for (const std::string metrica :
         {"cpu", "cpu.usage_pct", "ram", "disk", "rx_bytes", "tx_bytes"}) {
        EXPECT_NO_THROW(AlertaUmbral(metrica, 50.0f, '>')) << metrica;
        EXPECT_NO_THROW(AlertaUmbral(metrica, 50.0f, '<')) << metrica;
    }
}

TEST(AlertaUmbralTest, ValorActualDevuelveElCampoDeLaMetrica)
{
    const MetricSnapshot snap(10.0, 20.0, 30.0, 40.0, 50.0);

    EXPECT_DOUBLE_EQ(AlertaUmbral("cpu", 1.0f, '>').valorActual(snap), 10.0);
    EXPECT_DOUBLE_EQ(AlertaUmbral("cpu.usage_pct", 1.0f, '>').valorActual(snap), 10.0);
    EXPECT_DOUBLE_EQ(AlertaUmbral("ram", 1.0f, '>').valorActual(snap), 20.0);
    EXPECT_DOUBLE_EQ(AlertaUmbral("disk", 1.0f, '>').valorActual(snap), 30.0);
    EXPECT_DOUBLE_EQ(AlertaUmbral("rx_bytes", 1.0f, '>').valorActual(snap), 40.0);
    EXPECT_DOUBLE_EQ(AlertaUmbral("tx_bytes", 1.0f, '>').valorActual(snap), 50.0);
}

TEST(AlertaUmbralTest, GettersRetornanLosDatosDeConstruccion)
{
    AlertaUmbral alerta("ram", 75.5f, '<');

    EXPECT_EQ(alerta.metrica(), "ram");
    EXPECT_FLOAT_EQ(alerta.umbral(), 75.5f);
    EXPECT_EQ(alerta.operador(), '<');
}

TEST(AlertaUmbralTest, OperadorMayorNoDisparaCuandoValorEsIgualAlUmbral)
{
    AlertaUmbral alerta("cpu", 50.0f, '>');

    MetricSnapshot snap;
    snap.cpu = 50.0;

    EXPECT_FALSE(alerta.evaluar(snap));
}

TEST(AlertaUmbralTest, OperadorMenorNoDisparaCuandoValorEsIgualAlUmbral)
{
    AlertaUmbral alerta("ram", 20.0f, '<');

    MetricSnapshot snap;
    snap.ram = 20.0;

    EXPECT_FALSE(alerta.evaluar(snap));
}

TEST(AlertaUmbralTest, EvaluarDiskDisparaCuandoSuperaUmbral)
{
    AlertaUmbral alerta("disk", 80.0f, '>');

    MetricSnapshot snap;
    snap.disk = 95.0;

    EXPECT_TRUE(alerta.evaluar(snap));
}

TEST(AlertaUmbralTest, EvaluarRxBytesDisparaCuandoSuperaUmbral)
{
    AlertaUmbral alerta("rx_bytes", 1000.0f, '>');

    MetricSnapshot snap;
    snap.rx_bytes = 2048.0;

    EXPECT_TRUE(alerta.evaluar(snap));
}

TEST(AlertaUmbralTest, EvaluarTxBytesDisparaCuandoSuperaUmbral)
{
    AlertaUmbral alerta("tx_bytes", 1000.0f, '>');

    MetricSnapshot snap;
    snap.tx_bytes = 2048.0;

    EXPECT_TRUE(alerta.evaluar(snap));
}

TEST(AlertaUmbralTest, AliasCpuEquivaleACpuUsagePct)
{
    MetricSnapshot snap;
    snap.cpu = 65.5;

    AlertaUmbral alertaAlias("cpu", 50.0f, '>');
    AlertaUmbral alertaCompleta("cpu.usage_pct", 50.0f, '>');

    EXPECT_EQ(alertaAlias.evaluar(snap), alertaCompleta.evaluar(snap));
}

TEST(AlertaUmbralTest, MensajeContieneMetricaYOperador)
{
    AlertaUmbral alerta("disk", 80.0f, '>');

    std::string msg = alerta.mensaje();

    EXPECT_NE(msg.find("disk"), std::string::npos);
    EXPECT_NE(msg.find('>'), std::string::npos);
}
