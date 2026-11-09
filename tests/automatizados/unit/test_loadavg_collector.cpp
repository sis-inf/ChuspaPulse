#include <cassert>
#include <string>
#include <sstream>
#include <cmath>

// Estructura para almacenar las métricas leídas
struct LoadAvg {
    double min1;
    double min5;
    double min15;
};

// Función de simulación que parsea la cadena de /proc/loadavg
bool parsear_loadavg(const std::string& linea, LoadAvg& resultado) {
    std::stringstream ss(linea);
    // Intentamos leer los primeros tres valores flotantes
    if (!(ss >> resultado.min1 >> resultado.min5 >> resultado.min15)) {
        return false;
    }
    return true;
}

// Función auxiliar para comparar números flotantes con precisión (épsilon)
bool son_casi_iguales(double a, double b) {
    return std::abs(a - b) < 0.001;
}

int main() {
    std::string linea_prueba = "0.50 0.75 1.20 1/450 12345";
    LoadAvg metricas = {0.0, 0.0, 0.0};

    // 1. Verificar que el parseo sea exitoso
    assert(parsear_loadavg(linea_prueba, metricas) == true);

    // 2. Verificar la precisión de los valores de 1, 5 y 15 minutos
    assert(son_casi_iguales(metricas.min1, 0.50));
    assert(son_casi_iguales(metricas.min5, 0.75));
    assert(son_casi_iguales(metricas.min15, 1.20));

    return 0;
}
