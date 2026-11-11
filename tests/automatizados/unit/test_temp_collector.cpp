#include <gtest/gtest.h>
#include <vector>
#include "../../../src/collectors/temperature/temp_collector.hpp"

// Prueba unitaria utilizando el framework nativo GoogleTest
TEST(TempCollectorTest, ValidarFiltroRangosTermicos) {
    TempCollector colector;

    // 1. Simular lecturas válidas dentro del rango físico (0 a 150)
    // Se asume un método como procesar_lectura o similar del objeto real
    EXPECT_TRUE(colector.procesar_lectura(0.0));
    EXPECT_TRUE(colector.procesar_lectura(25.5));
    EXPECT_TRUE(colector.procesar_lectura(150.0));

    // 2. Verificar que las lecturas inválidas (negativas) sean omitidas (devuelven false o se descartan)
    EXPECT_FALSE(colector.procesar_lectura(-1.0));
    EXPECT_FALSE(colector.procesar_lectura(-15.0));

    // 3. Verificar que las lecturas superiores a 150 grados sean omitidas
    EXPECT_FALSE(colector.procesar_lectura(150.1));
    EXPECT_FALSE(colector.procesar_lectura(200.0));
}
