# Guía: Cómo agregar un nuevo collector

Esta guía explica paso a paso cómo agregar un nuevo collector a ChuspaPulse, usando como ejemplo ficticio un **collector de temperatura de GPU**.

---

## ¿Qué es un collector?

Un collector es una clase que hereda de `ICollector` y es responsable de recolectar un conjunto de métricas del sistema. El sampler itera sobre todos los collectors registrados y llama a `recolectar()` en cada uno.

---

## Convención de nombres de métrica

Los nombres de métrica siguen el formato `categoria.campo` en minúsculas, usando punto como separador:
gpu.temperatura
gpu.uso_percent
bateria.porcentaje
cpu.uso_percent

Nunca uses mayúsculas, espacios ni guiones en los nombres de métrica.

---

## Paso 1: Crear la carpeta y los archivos del collector

Crea una carpeta para tu collector dentro de `src/collectors/`:
src/collectors/gpu/
├── gpu_collector.hpp
└── gpu_collector.cpp

### gpu_collector.hpp

```cpp
#pragma once
#include <string>
#include <vector>
#include "../icollector.hpp"
#include "core/types.hpp"

namespace ChuspaPulse::collectors::gpu {

/**
 * @brief Información de temperatura de la GPU.
 */
struct GpuInfo {
    double temperatura;  // Temperatura en grados Celsius
    bool disponible;     // false si no hay GPU o no se puede leer
};

/**
 * @brief Lee la temperatura de la GPU desde el sistema.
 *
 * @param ruta_sensor Ruta al archivo de temperatura (inyectable en tests).
 * @return GpuInfo con la temperatura leída, o disponible=false si falla.
 */
GpuInfo getGpuInfo(
    const std::string& ruta_sensor = "/sys/class/drm/card0/device/hwmon/hwmon0/temp1_input"
);

/**
 * @brief Collector de métricas de temperatura de GPU.
 *
 * En sistemas sin GPU o sin sensor accesible, recolectar()
 * retorna un vector vacío sin lanzar excepción.
 */
class CollectorGpu : public ChuspaPulse::collectors::ICollector {
public:
    std::string nombre() const override;
    std::vector<ChuspaPulse::core::Metrica> recolectar() override;
};

}  // namespace ChuspaPulse::collectors::gpu
```

### gpu_collector.cpp

```cpp
#include "gpu_collector.hpp"
#include <chrono>
#include <fstream>
#include <string>

namespace ChuspaPulse::collectors::gpu {

GpuInfo getGpuInfo(const std::string& ruta_sensor) {
    std::ifstream archivo(ruta_sensor);
    if (!archivo.is_open()) {
        return GpuInfo{0.0, false};
    }

    int valor_raw = 0;
    if (!(archivo >> valor_raw)) {
        return GpuInfo{0.0, false};
    }

    // El kernel reporta la temperatura en miligrados Celsius
    double temperatura = static_cast<double>(valor_raw) / 1000.0;
    return GpuInfo{temperatura, true};
}

std::string CollectorGpu::nombre() const {
    return "gpu";
}

std::vector<ChuspaPulse::core::Metrica> CollectorGpu::recolectar() {
    GpuInfo info = getGpuInfo();

    if (!info.disponible) {
        return {};
    }

    auto timestamp = static_cast<std::int64_t>(
        std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
        )
    );

    return {{
        "gpu.temperatura",
        info.temperatura,
        "celsius",
        timestamp
    }};
}

}  // namespace ChuspaPulse::collectors::gpu
```

---

## Paso 2: Registrar el archivo en CMakeLists.txt

Abre `src/CMakeLists.txt` y agrega tu archivo `.cpp` a la lista de `target_sources`:

```cmake
target_sources(ChuspaPulse PRIVATE
    # ... otros collectors ...
    collectors/gpu/gpu_collector.cpp   # <-- agregar esta línea
)
```

---

## Paso 3: Agregar el collector al vector en main.cpp

Abre `src/main.cpp` y agrega el include y la instancia de tu collector:

```cpp
// Agrega el include junto a los demás collectors
#include "collectors/gpu/gpu_collector.hpp"

// Agrega la instancia al vector de collectors
collectors.push_back(std::make_unique<ChuspaPulse::collectors::gpu::CollectorGpu>());
```

El sampler iterará automáticamente sobre tu collector junto con los demás.

---

## Paso 4: Escribir el test unitario

Crea el archivo `tests/automatizados/unit/test_collector_gpu.cpp`:

```cpp
#include <gtest/gtest.h>
#include "collectors/gpu/gpu_collector.hpp"

using namespace ChuspaPulse::collectors::gpu;

TEST(CollectorGpu, RetornaDisponibleFalseConRutaInexistente) {
    GpuInfo info = getGpuInfo("/ruta/que/no/existe");
    EXPECT_FALSE(info.disponible);
    EXPECT_EQ(info.temperatura, 0.0);
}

TEST(CollectorGpu, RecolectarRetornaVacioSiNoHayGpu) {
    CollectorGpu collector;
    // Sin GPU real en el entorno de CI, debe retornar vacío sin lanzar excepción
    EXPECT_NO_THROW({
        auto metricas = collector.recolectar();
        EXPECT_TRUE(metricas.empty());
    });
}

TEST(CollectorGpu, NombreEsCorrecto) {
    CollectorGpu collector;
    EXPECT_EQ(collector.nombre(), "gpu");
}

TEST(CollectorGpu, NombreMetricaSigueLaConvencion) {
    // La convención es categoria.campo en minúsculas con punto como separador
    CollectorGpu collector;
    // Si hubiera GPU disponible, la métrica se llamaría "gpu.temperatura"
    EXPECT_EQ(std::string("gpu.temperatura").find('.'), 3u);
}
```

Registra el test en `tests/CMakeLists.txt` siguiendo el patrón existente:

```cmake
add_executable(test_collector_gpu test_collector_gpu.cpp)
target_link_libraries(test_collector_gpu PRIVATE GTest::gtest_main ChuspaPulse_lib)
add_test(NAME test_collector_gpu COMMAND test_collector_gpu)
```

---

## Resumen de archivos a crear/modificar

| Acción | Archivo |
|--------|---------|
| Crear | `src/collectors/gpu/gpu_collector.hpp` |
| Crear | `src/collectors/gpu/gpu_collector.cpp` |
| Crear | `tests/automatizados/unit/test_collector_gpu.cpp` |
| Modificar | `src/CMakeLists.txt` — agregar el `.cpp` a `target_sources` |
| Modificar | `src/main.cpp` — agregar include e instancia al vector |

---

## Notas

- Si tu collector solo tiene sentido en Linux, envuélvelo en un `#ifdef __linux__` igual que el collector de CPU para Windows.
- Si el sensor no está disponible (caso común en CI o servidores), retorna un vector vacío desde `recolectar()` — nunca lances una excepción por ausencia de hardware.
- El collector se registrará automáticamente en el sampler con solo agregarlo al vector en `main.cpp`.