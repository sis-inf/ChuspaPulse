#include <gtest/gtest.h>

#include <string>
#include <vector>

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

// --- Eventos solo ante cambios de estado (issue #1529) ---

namespace {

struct Eventos {
    std::vector<std::string> mensajes;

    void conectar(GestorAlertas& gestor) {
        gestor.setCallback([this](const std::string& m) { mensajes.push_back(m); });
    }
};

MetricSnapshot conCpu(double cpu) { return MetricSnapshot(cpu, 10, 10, 10, 10); }

} // namespace

TEST(GestorAlertasEstadoTest, ActivacionSeNotificaUnaVez)
{
    GestorAlertas gestor;
    Eventos eventos;
    eventos.conectar(gestor);
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    gestor.evaluar(conCpu(80));

    ASSERT_EQ(eventos.mensajes.size(), 1u);
    EXPECT_EQ(eventos.mensajes[0].rfind("Alerta: cpu >", 0), 0u);
    EXPECT_TRUE(gestor.getAlertas()[0].activa());
}

TEST(GestorAlertasEstadoTest, PersistenciaNoRepiteLaNotificacion)
{
    GestorAlertas gestor;
    Eventos eventos;
    eventos.conectar(gestor);
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    gestor.evaluar(conCpu(80));
    gestor.evaluar(conCpu(85));
    gestor.evaluar(conCpu(90));

    EXPECT_EQ(eventos.mensajes.size(), 1u);
}

TEST(GestorAlertasEstadoTest, RecuperacionSeRegistra)
{
    GestorAlertas gestor;
    Eventos eventos;
    eventos.conectar(gestor);
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    gestor.evaluar(conCpu(80));
    gestor.evaluar(conCpu(20));
    gestor.evaluar(conCpu(20));

    ASSERT_EQ(eventos.mensajes.size(), 2u);
    EXPECT_EQ(eventos.mensajes[1].rfind("Alerta resuelta: cpu >", 0), 0u);
    EXPECT_FALSE(gestor.getAlertas()[0].activa());
}

TEST(GestorAlertasEstadoTest, ReactivacionVuelveANotificar)
{
    GestorAlertas gestor;
    Eventos eventos;
    eventos.conectar(gestor);
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    gestor.evaluar(conCpu(80)); // activa
    gestor.evaluar(conCpu(20)); // resuelta
    gestor.evaluar(conCpu(90)); // activa otra vez
    gestor.evaluar(conCpu(95)); // sigue activa: sin evento

    ASSERT_EQ(eventos.mensajes.size(), 3u);
    EXPECT_EQ(eventos.mensajes[0].rfind("Alerta: cpu >", 0), 0u);
    EXPECT_EQ(eventos.mensajes[1].rfind("Alerta resuelta: cpu >", 0), 0u);
    EXPECT_EQ(eventos.mensajes[2].rfind("Alerta: cpu >", 0), 0u);
}

TEST(GestorAlertasEstadoTest, SoloLasActivacionesSeGuardanEnLaBase)
{
    SQLite::Database db(":memory:", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db.exec(
        "CREATE TABLE alerts (id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "timestamp INTEGER NOT NULL, metrica TEXT NOT NULL, umbral REAL NOT NULL, "
        "valor REAL NOT NULL, mensaje TEXT NOT NULL);"
    );
    GestorAlertas gestor(db);
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));

    gestor.evaluar(conCpu(80)); // activa: se guarda
    gestor.evaluar(conCpu(85)); // sigue activa: no se guarda
    gestor.evaluar(conCpu(20)); // resuelta: no se guarda
    gestor.evaluar(conCpu(90)); // reactivada: se guarda

    const auto historial = gestor.obtenerHistorial();
    ASSERT_EQ(historial.size(), 2u);
    EXPECT_DOUBLE_EQ(historial[0].valor, 90.0);
    EXPECT_DOUBLE_EQ(historial[1].valor, 80.0);
}

TEST(GestorAlertasEstadoTest, AlertasDistintasTienenEstadosIndependientes)
{
    GestorAlertas gestor;
    Eventos eventos;
    eventos.conectar(gestor);
    gestor.addAlerta(AlertaUmbral("cpu", 50, '>'));
    gestor.addAlerta(AlertaUmbral("ram", 50, '>'));

    gestor.evaluar(MetricSnapshot(80, 10, 10, 10, 10)); // solo cpu se activa
    gestor.evaluar(MetricSnapshot(80, 70, 10, 10, 10)); // cpu sigue, ram se activa
    gestor.evaluar(MetricSnapshot(20, 70, 10, 10, 10)); // cpu se resuelve, ram sigue

    ASSERT_EQ(eventos.mensajes.size(), 3u);
    EXPECT_EQ(eventos.mensajes[0].rfind("Alerta: cpu >", 0), 0u);
    EXPECT_EQ(eventos.mensajes[1].rfind("Alerta: ram >", 0), 0u);
    EXPECT_EQ(eventos.mensajes[2].rfind("Alerta resuelta: cpu >", 0), 0u);
    EXPECT_FALSE(gestor.getAlertas()[0].activa());
    EXPECT_TRUE(gestor.getAlertas()[1].activa());
}