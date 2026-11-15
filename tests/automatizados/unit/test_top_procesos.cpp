#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "collectors/procesos/top_procesos.hpp"

namespace {

using chuspapulse::collectors::procesos::ProcesoInfo;

/**
 * @brief Construye un proceso simulado con el uso de CPU indicado.
 * @param pid Identificador del proceso.
 * @param nombre Nombre del ejecutable.
 * @param cpu Uso de CPU en porcentaje.
 */
ProcesoInfo crearProceso(int32_t pid, const std::string& nombre, double cpu) {
    ProcesoInfo proceso{};
    proceso.pid = pid;
    proceso.nombre = nombre;
    proceso.cpu_porcentaje = cpu;
    proceso.memoria_rss_bytes = 0;
    return proceso;
}

/**
 * @brief Ordena los procesos por uso de CPU decreciente y acota la lista.
 * @param procesos Lista de procesos a ordenar.
 * @param limite Cantidad maxima de procesos a devolver.
 */
std::vector<ProcesoInfo> ordenarYAcotar(std::vector<ProcesoInfo> procesos,
                                        std::size_t limite) {
    std::stable_sort(procesos.begin(), procesos.end(),
                     [](const ProcesoInfo& a, const ProcesoInfo& b) {
                         return a.cpu_porcentaje > b.cpu_porcentaje;
                     });

    if (procesos.size() > limite) {
        procesos.resize(limite);
    }

    return procesos;
}

/**
 * @brief El primer elemento es el de mayor consumo de CPU.
 */
TEST(TopProcesosTest, PrimerElementoEsElDeMayorCpu) {
    const std::vector<ProcesoInfo> procesos = {
        crearProceso(101, "bash", 3.5),
        crearProceso(102, "firefox", 87.2),
        crearProceso(103, "systemd", 0.4),
        crearProceso(104, "code", 41.9),
    };

    const std::vector<ProcesoInfo> resultado = ordenarYAcotar(procesos, 5);

    ASSERT_FALSE(resultado.empty());
    EXPECT_EQ(102, resultado.front().pid);
    EXPECT_EQ("firefox", resultado.front().nombre);
    EXPECT_DOUBLE_EQ(87.2, resultado.front().cpu_porcentaje);
}

/**
 * @brief El orden es estrictamente decreciente por uso de CPU.
 */
TEST(TopProcesosTest, OrdenEsDecrecientePorCpu) {
    const std::vector<ProcesoInfo> procesos = {
        crearProceso(201, "a", 10.0),
        crearProceso(202, "b", 55.5),
        crearProceso(203, "c", 2.25),
        crearProceso(204, "d", 99.9),
        crearProceso(205, "e", 30.0),
    };

    const std::vector<ProcesoInfo> resultado = ordenarYAcotar(procesos, 5);

    ASSERT_EQ(static_cast<std::size_t>(5), resultado.size());
    for (std::size_t i = 1; i < resultado.size(); ++i) {
        EXPECT_GE(resultado[i - 1].cpu_porcentaje, resultado[i].cpu_porcentaje);
    }
}

/**
 * @brief La lista devuelta respeta el limite solicitado.
 */
TEST(TopProcesosTest, ListaSeMantieneAcotada) {
    std::vector<ProcesoInfo> procesos;
    for (int32_t i = 0; i < 20; ++i) {
        procesos.push_back(crearProceso(300 + i, "proc", static_cast<double>(i)));
    }

    const std::vector<ProcesoInfo> resultado = ordenarYAcotar(procesos, 5);

    EXPECT_EQ(static_cast<std::size_t>(5), resultado.size());
}

/**
 * @brief Al acotar se conservan los procesos de mayor CPU, no los primeros.
 */
TEST(TopProcesosTest, AlAcotarSeConservanLosDeMayorCpu) {
    const std::vector<ProcesoInfo> procesos = {
        crearProceso(401, "bajo", 1.0),
        crearProceso(402, "medio", 50.0),
        crearProceso(403, "alto", 90.0),
        crearProceso(404, "minimo", 0.5),
    };

    const std::vector<ProcesoInfo> resultado = ordenarYAcotar(procesos, 2);

    ASSERT_EQ(static_cast<std::size_t>(2), resultado.size());
    EXPECT_EQ(403, resultado[0].pid);
    EXPECT_EQ(402, resultado[1].pid);
}

/**
 * @brief Una lista mas corta que el limite se devuelve completa.
 */
TEST(TopProcesosTest, ListaMasCortaQueElLimiteSeDevuelveCompleta) {
    const std::vector<ProcesoInfo> procesos = {
        crearProceso(501, "uno", 12.0),
        crearProceso(502, "dos", 80.0),
    };

    const std::vector<ProcesoInfo> resultado = ordenarYAcotar(procesos, 5);

    ASSERT_EQ(static_cast<std::size_t>(2), resultado.size());
    EXPECT_EQ(502, resultado.front().pid);
}

/**
 * @brief Una lista vacia devuelve una lista vacia.
 */
TEST(TopProcesosTest, ListaVaciaDevuelveListaVacia) {
    const std::vector<ProcesoInfo> procesos;

    EXPECT_TRUE(ordenarYAcotar(procesos, 5).empty());
}

/**
 * @brief Un solo proceso queda como primer elemento.
 */
TEST(TopProcesosTest, UnSoloProcesoQuedaPrimero) {
    const std::vector<ProcesoInfo> procesos = {
        crearProceso(601, "unico", 7.75),
    };

    const std::vector<ProcesoInfo> resultado = ordenarYAcotar(procesos, 5);

    ASSERT_EQ(static_cast<std::size_t>(1), resultado.size());
    EXPECT_EQ(601, resultado.front().pid);
    EXPECT_DOUBLE_EQ(7.75, resultado.front().cpu_porcentaje);
}

} // namespace
