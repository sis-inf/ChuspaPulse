# Guía de Pruebas Unitarias con Google Test

## Introducción

Google Test es el framework de pruebas utilizado en ChuspaPulse para verificar el comportamiento correcto de los componentes del sistema. Permite escribir pruebas unitarias legibles, automatizadas y fáciles de mantener.

Las pruebas ayudan a detectar errores antes de que lleguen a producción y facilitan la refactorización segura del código.

---

## Google Test básico

Las pruebas se escriben utilizando la macro `TEST`.

Ejemplo:

```cpp
#include <gtest/gtest.h>

TEST(SumaTest, SumaDosNumeros)
{
    EXPECT_EQ(2 + 2, 4);
}
```

### ASSERT vs EXPECT

Google Test proporciona dos familias principales de verificaciones:

#### EXPECT

Permite que la prueba continúe ejecutándose aunque la condición falle.

```cpp
EXPECT_EQ(resultado, esperado);
```

#### ASSERT

Detiene inmediatamente la ejecución de la prueba actual si la condición falla.

```cpp
ASSERT_EQ(resultado, esperado);
```

Utilice `ASSERT` cuando no tenga sentido continuar la prueba después de un error.

### EXPECT_THROW

Permite verificar que una operación genera una excepción.

```cpp
EXPECT_THROW(
    dividir(10, 0),
    std::runtime_error
);
```

---

## Fixtures

Las fixtures permiten compartir configuración entre varias pruebas.

Se implementan mediante una clase derivada de `testing::Test`.

### SetUp y TearDown

* `SetUp()` se ejecuta antes de cada prueba.
* `TearDown()` se ejecuta después de cada prueba.

Ejemplo:

```cpp
class CollectorCPUTest : public testing::Test
{
protected:
    void SetUp() override
    {
        rutaFixture = "tests/fixtures/proc_stat_sample";
    }

    void TearDown() override
    {
    }

    std::string rutaFixture;
};
```

Las pruebas utilizan la macro `TEST_F`.

```cpp
TEST_F(CollectorCPUTest, ArchivoExiste)
{
    ASSERT_FALSE(rutaFixture.empty());
}
```

---

## Tests con archivos mock

Para evitar depender del sistema operativo real, las pruebas deben utilizar archivos controlados dentro de:

```text
tests/fixtures/
```

Ejemplo:

```text
tests/
└── fixtures/
    └── proc_stat_sample
```

Esto permite obtener resultados reproducibles en cualquier entorno.

---

## Ejemplo completo: CollectorCPU

```cpp
#include <gtest/gtest.h>
#include "CollectorCPU.h"

class CollectorCPUTest : public testing::Test
{
protected:
    void SetUp() override
    {
        archivo = "tests/fixtures/proc_stat_sample";
    }

    std::string archivo;
};

TEST_F(
    CollectorCPUTest,
    LeeCorrectamenteProcStat
)
{
    CollectorCPU collector;

    double usoCPU =
        collector.obtenerUsoCPU(archivo);

    EXPECT_GE(usoCPU, 0.0);
    EXPECT_LE(usoCPU, 100.0);
}
```

Este ejemplo utiliza una fixture para reutilizar la ubicación del archivo de prueba y validar que el valor obtenido se encuentre dentro de un rango válido.

---

## Compilar y ejecutar pruebas

Una vez compilado el proyecto, las pruebas pueden ejecutarse mediante:

```bash
ctest
```

Para obtener más detalle:

```bash
ctest --output-on-failure
```

Esto mostrará los errores producidos por las pruebas que fallen.

---

## Convenciones

### Nombre de suites

Utilizar nombres relacionados con el componente probado.

Ejemplos:

```cpp
CollectorCPUTest
ParserConfigTest
GestorAlertasTest
```

### Nombre de pruebas

Describir claramente el comportamiento esperado.

Ejemplos:

```cpp
LeeCorrectamenteProcStat
CargaConfiguracionValida
GeneraAlertaCuandoSuperaUmbral
```

### Buenas prácticas

* Una prueba debe verificar un único comportamiento.
* Utilizar fixtures para evitar código duplicado.
* Mantener las pruebas independientes.
* Utilizar archivos dentro de `tests/fixtures`.
* Evitar dependencias externas durante la ejecución.

---

## Conclusión

Google Test proporciona una forma sencilla y robusta de verificar el comportamiento de ChuspaPulse. El uso correcto de `ASSERT`, `EXPECT`, fixtures y archivos mock permite construir una suite de pruebas confiable, mantenible y fácil de ejecutar mediante `ctest`.
