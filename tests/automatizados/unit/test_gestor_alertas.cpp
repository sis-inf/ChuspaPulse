#include <gtest/gtest.h>

#include "alertas/gestor_alertas.hpp"
#include "alertas/alerta_umbral.hpp"
#include "core/MetricSnapshot.hpp"

using namespace pulso;
using namespace pulso::alertas;

TEST(GestorAlertasTest, CallbackSeLlamaCuandoAlertaSeDispara)
{
    GestorAlertas gestor;

    int llamadas = 0;

    gestor.setCallback([&](const std::string&) {
        llamadas++;
    });

    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    MetricSnapshot snap(80, 10, 10, 10, 10);

    gestor.evaluar(snap);

    EXPECT_EQ(llamadas, 1);
}

TEST(GestorAlertasTest, CallbackNoSeLlamaCuandoNoSeDisparaAlerta)
{
    GestorAlertas gestor;

    int llamadas = 0;

    gestor.setCallback([&](const std::string&) {
        llamadas++;
    });

    gestor.addAlerta(AlertaUmbral("cpu", 90, '>'));

    MetricSnapshot snap(50, 10, 10, 10, 10);

    gestor.evaluar(snap);

    EXPECT_EQ(llamadas, 0);
}

TEST(GestorAlertasTest, DosAlertasActivasLlamanDosVeces)
{
    GestorAlertas gestor;

    int llamadas = 0;

    gestor.setCallback([&](const std::string&) {
        llamadas++;
    });

    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));
    gestor.addAlerta(AlertaUmbral("ram", 50, '>'));

    MetricSnapshot snap(80, 80, 10, 10, 10);

    gestor.evaluar(snap);

    EXPECT_EQ(llamadas, 2);
}

TEST(GestorAlertasTest, EvaluarSinAlertasNoFalla)
{
    GestorAlertas gestor;

    gestor.setCallback([](const std::string&) {});

    MetricSnapshot snap(10, 10, 10, 10, 10);

    EXPECT_NO_THROW(
        gestor.evaluar(snap);
    );
}
TEST(GestorAlertasTest, ValorIgualAlUmbralNoDisparaConOperadorMayor)
{
    GestorAlertas gestor;
    int llamadas = 0;
    gestor.setCallback([&](const std::string&) { llamadas++; });
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    MetricSnapshot snap(50, 10, 10, 10, 10);
    gestor.evaluar(snap);

    EXPECT_EQ(llamadas, 0);
}

TEST(GestorAlertasTest, ValorIgualAlUmbralNoDisparaConOperadorMenor)
{
    GestorAlertas gestor;
    int llamadas = 0;
    gestor.setCallback([&](const std::string&) { llamadas++; });
    gestor.addAlerta(AlertaUmbral("cpu", 50, '<'));

    MetricSnapshot snap(50, 10, 10, 10, 10);
    gestor.evaluar(snap);

    EXPECT_EQ(llamadas, 0);
}

TEST(GestorAlertasTest, ValorJustoPorEncimaDelUmbralDisparaConOperadorMayor)
{
    GestorAlertas gestor;
    int llamadas = 0;
    gestor.setCallback([&](const std::string&) { llamadas++; });
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    MetricSnapshot snap(51, 10, 10, 10, 10);
    gestor.evaluar(snap);

    EXPECT_EQ(llamadas, 1);
}

TEST(GestorAlertasTest, ValorJustoPorDebajoDelUmbralDisparaConOperadorMenor)
{
    GestorAlertas gestor;
    int llamadas = 0;
    gestor.setCallback([&](const std::string&) { llamadas++; });
    gestor.addAlerta(AlertaUmbral("cpu", 50, '<'));

    MetricSnapshot snap(49, 10, 10, 10, 10);
    gestor.evaluar(snap);

    EXPECT_EQ(llamadas, 1);
}

TEST(GestorAlertasTest, AliasCpuUsagePctSeEvaluaComoCpu)
{
    GestorAlertas gestor;
    int llamadas = 0;
    gestor.setCallback([&](const std::string&) { llamadas++; });
    gestor.addAlerta(AlertaUmbral("cpu.usage_pct", 50, '>'));

    MetricSnapshot snap(80, 10, 10, 10, 10);
    gestor.evaluar(snap);

    EXPECT_EQ(llamadas, 1);
}

TEST(GestorAlertasTest, MetricaDesconocidaLanzaInvalidArgumentYNoLlamaAlCallback)
{
    GestorAlertas gestor;
    int llamadas = 0;
    gestor.setCallback([&](const std::string&) { llamadas++; });
    gestor.addAlerta(AlertaUmbral("gpu", 50, '>'));

    MetricSnapshot snap(80, 80, 80, 80, 80);

    EXPECT_THROW(gestor.evaluar(snap), std::invalid_argument);
    EXPECT_EQ(llamadas, 0);
}

TEST(GestorAlertasTest, SinCallbackNoEvaluaAlertasConMetricaDesconocida)
{
    GestorAlertas gestor;
    gestor.addAlerta(AlertaUmbral("gpu", 50, '>'));

    MetricSnapshot snap(80, 80, 80, 80, 80);

    EXPECT_NO_THROW(gestor.evaluar(snap));
}

TEST(GestorAlertasTest, CallbackVacioNoFalla)
{
    GestorAlertas gestor;
    gestor.setCallback(nullptr);
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    MetricSnapshot snap(80, 10, 10, 10, 10);

    EXPECT_NO_THROW(gestor.evaluar(snap));
}

TEST(GestorAlertasTest, CallbackReemplazadoSoloNotificaAlUltimo)
{
    GestorAlertas gestor;
    int llamadasPrimero = 0;
    int llamadasSegundo = 0;
    gestor.setCallback([&](const std::string&) { llamadasPrimero++; });
    gestor.setCallback([&](const std::string&) { llamadasSegundo++; });
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    MetricSnapshot snap(80, 10, 10, 10, 10);
    gestor.evaluar(snap);

    EXPECT_EQ(llamadasPrimero, 0);
    EXPECT_EQ(llamadasSegundo, 1);
}

TEST(GestorAlertasTest, CallbackQueLanzaExcepcionLaPropaga)
{
    GestorAlertas gestor;
    gestor.setCallback([](const std::string&) {
        throw std::runtime_error("fallo en el callback");
    });
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    MetricSnapshot snap(80, 10, 10, 10, 10);

    EXPECT_THROW(gestor.evaluar(snap), std::runtime_error);
}

TEST(GestorAlertasTest, OperadorInvalidoLanzaInvalidArgumentAlCrearLaAlerta)
{
    EXPECT_THROW(AlertaUmbral("cpu", 50, '='), std::invalid_argument);
    EXPECT_THROW(AlertaUmbral("cpu", 50, 'x'), std::invalid_argument);
}

TEST(GestorAlertasTest, CantidadAlertasCuentaLasAlertasRegistradas)
{
    GestorAlertas gestor;
    EXPECT_EQ(gestor.cantidadAlertas(), 0u);

    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));
    gestor.addAlerta(AlertaUmbral("ram", 50, '>'));

    EXPECT_EQ(gestor.cantidadAlertas(), 2u);
}

TEST(GestorAlertasTest, AlertaDuplicadaSeNotificaDosVeces)
{
    GestorAlertas gestor;
    int llamadas = 0;
    gestor.setCallback([&](const std::string&) { llamadas++; });
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    MetricSnapshot snap(80, 10, 10, 10, 10);
    gestor.evaluar(snap);

    EXPECT_EQ(llamadas, 2);
}

TEST(GestorAlertasTest, CallbackRecibeElMensajeDeLaAlerta)
{
    GestorAlertas gestor;
    std::string recibido;
    gestor.setCallback([&](const std::string& mensaje) { recibido = mensaje; });

    AlertaUmbral alerta("cpu", 50, '>');
    gestor.addAlerta(alerta);

    MetricSnapshot snap(80, 10, 10, 10, 10);
    gestor.evaluar(snap);

    EXPECT_EQ(recibido, alerta.mensaje());
    EXPECT_NE(recibido.find("cpu"), std::string::npos);
}
