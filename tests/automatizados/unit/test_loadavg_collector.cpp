#include <cassert>
#include <string>
#include <cmath>
#include "../../../src/colectores/loadavg_collector.hpp" 

bool son_casi_iguales(double a, double b) {
    return std::abs(a - b) < 0.001;
}

int main() {
    std::string linea_prueba = "0.50 0.75 1.20 1/450 12345";

    // Crea la instancia usando el objeto real del proyecto
    LoadavgCollector colector;

    // Ejecuta el método real que parsea la línea en el agente
    bool exito = colector.parsear_linea(linea_prueba);
    
    assert(exito == true);

    // Comprobar con precisión matemática los 3 valores reales extraídos
    assert(son_casi_iguales(colector.get_min1(), 0.50));
    assert(son_casi_iguales(colector.get_min5(), 0.75));
    assert(son_casi_iguales(colector.get_min15(), 1.20));

    return 0;
}
