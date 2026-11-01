#include <gtest/gtest.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "alertas/gestor_alertas.hpp"
#include "core/MetricSnapshot.hpp"
#include "storage/schema.hpp"

class AlertasPersistencia : public ::testing::Test {
protected:
    AlertasPersistencia()
        : db(":memory:", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE), gestor(db) {
        chuspapulse::storage::inicializarEsquema(db);
    }

    int contarAlertas() {
        SQLite::Statement q(db, "SELECT COUNT(*) FROM alerts");
        q.executeStep();
        return q.getColumn(0).getInt();
    }

    SQLite::Database db;
    chuspapulse::alertas::GestorAlertas gestor;
};

TEST_F(AlertasPersistencia, AlertaDisparadaQuedaEnLaBase) {
    gestor.addAlerta(chuspapulse::alertas::AlertaUmbral("cpu", 80.0f, '>'));

    chuspapulse::MetricSnapshot snap;
    snap.cpu = 85.0;
    gestor.evaluar(snap);

    EXPECT_EQ(contarAlertas(), 1);
    auto historial = gestor.obtenerHistorial();
    ASSERT_EQ(historial.size(), 1u);
    EXPECT_EQ(historial[0].metrica, "cpu");
    EXPECT_DOUBLE_EQ(historial[0].umbral, 80.0);
    EXPECT_DOUBLE_EQ(historial[0].valor, 85.0);
}

TEST_F(AlertasPersistencia, SinDisparoNoSeGuardaNada) {
    gestor.addAlerta(chuspapulse::alertas::AlertaUmbral("cpu", 80.0f, '>'));

    chuspapulse::MetricSnapshot snap;
    snap.cpu = 10.0;
    gestor.evaluar(snap);

    EXPECT_EQ(contarAlertas(), 0);
}

TEST_F(AlertasPersistencia, HistorialOrdenadoPorTimestampYPaginado) {
    db.exec("INSERT INTO alerts (timestamp, metrica, umbral, valor, mensaje) VALUES (100,'cpu',80,90,'a')");
    db.exec("INSERT INTO alerts (timestamp, metrica, umbral, valor, mensaje) VALUES (300,'ram',80,95,'b')");
    db.exec("INSERT INTO alerts (timestamp, metrica, umbral, valor, mensaje) VALUES (200,'disk',80,85,'c')");

    auto todo = gestor.obtenerHistorial();
    ASSERT_EQ(todo.size(), 3u);
    EXPECT_EQ(todo[0].timestamp, 300);
    EXPECT_EQ(todo[2].timestamp, 100);

    auto pagina = gestor.obtenerHistorial(1, 1);
    ASSERT_EQ(pagina.size(), 1u);
    EXPECT_EQ(pagina[0].timestamp, 200);
}