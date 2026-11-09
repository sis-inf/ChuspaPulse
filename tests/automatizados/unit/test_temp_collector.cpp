#include <cassert>
#include <iostream>

// Función para validar rangos térmicos
bool es_temperatura_valida(double temperatura) {
    // Filtra valores negativos y temperaturas superiores a 150 grados
    if (temperatura < 0.0 || temperatura > 150.0) {
        return false;
    }
    return true;
}

// Pruebas unitarias para el recolector de temperatura
void test_filtro_rangos_termicos() {
    // 1. Verificar que los valores dentro del rango (0 a 150) se mantengan válidos
    assert(es_temperatura_valida(0.0) == true);
    assert(es_temperatura_valida(25.5) == true);
    assert(es_temperatura_valida(150.0) == true);

    // 2. Verificar que se filtren/descarten los valores negativos
    assert(es_temperatura_valida(-1.0) == false);
    assert(es_temperatura_valida(-50.0) == false);

    // 3. Verificar que se filtren/descarten valores superiores a 150 grados
    assert(es_temperatura_valida(150.1) == false);
    assert(es_temperatura_valida(200.0) == false);

    std::cout << "Todas las pruebas de rangos termicos pasaron con exito." << std::endl;
}

int main() {
    test_filtro_rangos_termicos();
    return 0;
}
