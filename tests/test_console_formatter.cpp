#include "utils/console_formatter.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string>

// Declaración de la función
void printMetrics(const std::map<std::string, double> &data);

TEST(ConsoleFormatterTest, OutputContainsMetrics) {
  std::map<std::string, double> data = {{"CPU", 25.5}, {"RAM", 40.0}};

  std::ostringstream output;

  // Redirigir std::cout
  auto *oldCout = std::cout.rdbuf(output.rdbuf());

  // Llamar formateador con datos mock
  printMetrics(data);

  // Limpiar/restaurar stream
  std::cout.rdbuf(oldCout);

  std::string result = output.str();

  EXPECT_NE(result.find("CPU:"), std::string::npos);
  EXPECT_NE(result.find("RAM:"), std::string::npos);
}

static chuspapulse::core::Snapshot crearSnapshotConMetricas() {
  const double gigabyte = 1024.0 * 1024.0 * 1024.0;
  return {1700000000,
          {{"cpu.usage", 25.5, "%", 1700000000},
           {"ram.used", 2.0 * gigabyte, "bytes", 1700000000},
           {"ram.total", 8.0 * gigabyte, "bytes", 1700000000},
           {"disk.used", 120.0 * gigabyte, "bytes", 1700000000},
           {"disk.total", 500.0 * gigabyte, "bytes", 1700000000},
           {"network.rx", 1280.0, "bytes", 1700000000},
           {"network.tx", 768.0, "bytes", 1700000000}}};
}

static std::string obtenerFila(const std::string &tabla,
                               const std::string &etiqueta) {
  const auto inicio = tabla.find("| " + etiqueta);
  if (inicio == std::string::npos) {
    return "";
  }
  return tabla.substr(inicio, tabla.find('\n', inicio) - inicio);
}
TEST(ConsoleFormatterTest, TablaIncluyeEncabezados) {
  const auto tabla = formatAsTable(crearSnapshotConMetricas());

  EXPECT_NE(tabla.find("Métrica"), std::string::npos);
  EXPECT_NE(tabla.find("Valor"), std::string::npos);
  EXPECT_NE(tabla.find("Uso"), std::string::npos);
}

TEST(ConsoleFormatterTest, TablaMuestraMetricasConSusValoresYUnidades) {
  const auto tabla = formatAsTable(crearSnapshotConMetricas());

  EXPECT_NE(obtenerFila(tabla, "CPU").find("25.5 %"), std::string::npos);
  EXPECT_NE(obtenerFila(tabla, "RAM").find("2.0 / 8.0 GB"), std::string::npos);
  EXPECT_NE(obtenerFila(tabla, "Disco").find("120.0 / 500.0 GB"),
            std::string::npos);
  EXPECT_NE(obtenerFila(tabla, "Red").find("1.2 / 0.8 KB/s"),
            std::string::npos);
  EXPECT_NE(obtenerFila(tabla, "CPU").find("[#####-------------]"),
            std::string::npos);
}

TEST(ConsoleFormatterTest, SnapshotVacioNoLanzaExcepciones) {
  const chuspapulse::core::Snapshot snapshot{};
  std::string tabla;

  EXPECT_NO_THROW(tabla = formatAsTable(snapshot));
  EXPECT_FALSE(tabla.empty());
  EXPECT_NE(obtenerFila(tabla, "CPU").find("0.0 %"), std::string::npos);
  EXPECT_NE(obtenerFila(tabla, "RAM").find("0.0 / 0.0 GB"), std::string::npos);
  EXPECT_NE(obtenerFila(tabla, "Disco").find("0.0 / 0.0 GB"),
            std::string::npos);
  EXPECT_NE(obtenerFila(tabla, "Red").find("0.0 / 0.0 KB/s"),
            std::string::npos);
}

TEST(ConsoleFormatterTest, TotalesCeroProducenBarrasVacias) {
  auto snapshot = crearSnapshotConMetricas();
  snapshot.metricas[2].valor = 0.0;
  snapshot.metricas[4].valor = 0.0;
  const auto tabla = formatAsTable(snapshot);

  EXPECT_NE(obtenerFila(tabla, "RAM").find("[------------------]"),
            std::string::npos);
  EXPECT_NE(obtenerFila(tabla, "Disco").find("[------------------]"),
            std::string::npos);
  EXPECT_EQ(tabla.find("nan"), std::string::npos);
  EXPECT_EQ(tabla.find("inf"), std::string::npos);
}

TEST(ConsoleFormatterTest, BarraCpuRespetaLosLimitesDePorcentaje) {
  auto snapshot = crearSnapshotConMetricas();
  snapshot.metricas[0].valor = 150.0;
  const auto tablaSuperior = formatAsTable(snapshot);
  EXPECT_NE(obtenerFila(tablaSuperior, "CPU").find("[##################]"),
            std::string::npos);

  snapshot.metricas[0].valor = -20.0;
  const auto tablaInferior = formatAsTable(snapshot);
  EXPECT_NE(obtenerFila(tablaInferior, "CPU").find("[------------------]"),
            std::string::npos);
}

TEST(ConsoleFormatterTest, AliasDeMetricasConservanLaTabla) {
  const auto original = crearSnapshotConMetricas();
  auto alias = original;
  alias.metricas[0].nombre = "cpu";
  alias.metricas[1].nombre = "ram.usado";
  alias.metricas[3].nombre = "disco.usado";
  alias.metricas[4].nombre = "disco.total";
  alias.metricas[5].nombre = "net.rx";
  alias.metricas[6].nombre = "net.tx";

  EXPECT_EQ(formatAsTable(alias), formatAsTable(original));
}
