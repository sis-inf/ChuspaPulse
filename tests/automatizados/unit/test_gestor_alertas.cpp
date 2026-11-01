#include <gtest/gtest.h>

#include "alertas/gestor_alertas.hpp"
#include "alertas/alerta_umbral.hpp"
#include "core/MetricSnapshot.hpp"

using namespace chuspapulse;
using namespace chuspapulse::alertas;

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