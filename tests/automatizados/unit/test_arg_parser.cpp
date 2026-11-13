#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cli/arg_parser.h"

namespace {

struct Resultado {
    bool continuar = false;
    pulso::cli::CliOptions opciones;
    std::string salida;
    std::string errores;
};

// Ejecuta parse_arguments con los argumentos dados y captura stdout y stderr.
Resultado ejecutar(const std::vector<std::string>& args)
{
    std::vector<std::string> copia = args;
    std::vector<char*> argv;
    for (auto& a : copia) {
        argv.push_back(a.data());
    }

    Resultado r;
    std::ostringstream out;
    std::ostringstream err;
    std::streambuf* out_original = std::cout.rdbuf(out.rdbuf());
    std::streambuf* err_original = std::cerr.rdbuf(err.rdbuf());

    r.continuar = pulso::cli::parse_arguments(
        static_cast<int>(argv.size()), argv.data(), r.opciones);

    std::cout.rdbuf(out_original);
    std::cerr.rdbuf(err_original);

    r.salida = out.str();
    r.errores = err.str();
    return r;
}

} // namespace

TEST(ArgParserHelp, LargoImprimeAyudaYMarcaHelpRequested)
{
    auto r = ejecutar({"pulso", "--help"});

    EXPECT_TRUE(r.continuar);                 // no es un error: main sale con 0
    EXPECT_TRUE(r.opciones.help_requested);
    EXPECT_NE(r.salida.find("Uso:"), std::string::npos);
    EXPECT_NE(r.salida.find("--help"), std::string::npos);
    EXPECT_TRUE(r.errores.empty());
}

TEST(ArgParserHelp, CortoTieneElMismoComportamiento)
{
    auto r = ejecutar({"pulso", "-h"});

    EXPECT_TRUE(r.continuar);
    EXPECT_TRUE(r.opciones.help_requested);
    EXPECT_NE(r.salida.find("Uso:"), std::string::npos);
}

TEST(ArgParserHelp, NoProcesaArgumentosPosterioresAHelp)
{
    auto r = ejecutar({"pulso", "--help", "--no-existe"});

    EXPECT_TRUE(r.continuar);
    EXPECT_TRUE(r.opciones.help_requested);
    EXPECT_TRUE(r.errores.empty());
}

TEST(ArgParserHelp, ArgumentoDesconocidoSigueSiendoError)
{
    auto r = ejecutar({"pulso", "--no-existe"});

    EXPECT_FALSE(r.continuar);
    EXPECT_FALSE(r.opciones.help_requested);
    EXPECT_NE(r.errores.find("argumento desconocido"), std::string::npos);
}

TEST(ArgParserHelp, SinArgumentosNoPideAyuda)
{
    auto r = ejecutar({"pulso"});

    EXPECT_TRUE(r.continuar);
    EXPECT_FALSE(r.opciones.help_requested);
    EXPECT_TRUE(r.salida.empty());
}