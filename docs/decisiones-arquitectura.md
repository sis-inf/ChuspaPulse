# Decisiones de Arquitectura (ADR)

Este documento registra las decisiones de diseño más importantes del proyecto ChuspaPulse. Cada entrada sigue el formato estándar de Architecture Decision Record (ADR).

---

## ADR-001: Uso de SQLite como motor de almacenamiento

**Estado:** Aceptado

### Contexto

ChuspaPulse necesita persistir métricas históricas del sistema para permitir consultas, exportaciones y análisis posteriores. Se evaluaron distintas opciones: almacenamiento en memoria, archivos CSV, bases de datos embebidas y bases de datos de servidor.

### Decisión

Se usa **SQLite** como motor de almacenamiento local mediante la biblioteca `SQLiteCpp`. El archivo de base de datos se configura en `chuspapulse.toml` bajo la clave `ruta_db`.

### Consecuencias

- ✅ Sin dependencias de servidor: SQLite es una biblioteca embebida, no requiere procesos externos.
- ✅ Un solo archivo portable (`chuspapulse.db`) fácil de respaldar y mover.
- ✅ Soporte completo de SQL para consultas y exportaciones.
- ⚠️ No apto para escritura concurrente desde múltiples procesos simultáneos.
- ⚠️ Las migraciones de esquema deben hacerse manualmente (ver [docs/guia-migracion-esquema-bd.md](guia-migracion-esquema-bd.md)).

---

## ADR-002: Patrón ICollector para extensibilidad de fuentes de métricas

**Estado:** Aceptado

### Contexto

ChuspaPulse recolecta métricas de múltiples fuentes (CPU, RAM, disco, red, procesos, batería, temperatura). Se necesitaba un mecanismo que permitiera agregar nuevos collectors sin modificar el núcleo del sistema.

### Decisión

Se define la interfaz abstracta **ICollector** que todos los collectors deben implementar. El sistema itera sobre una lista de collectors registrados y llama a cada uno de forma independiente.

```cpp
class ICollector {
public:
    virtual std::vector collect() = 0;
    virtual std::string nombre() const = 0;
    virtual ~ICollector() = default;
};
```

### Consecuencias

- ✅ Agregar un nuevo collector no requiere modificar el núcleo — solo implementar `ICollector` y registrarlo.
- ✅ Cada collector es independiente y testeable por separado.
- ✅ Permite implementaciones específicas por plataforma (Linux vs Windows).
- ⚠️ Requiere que cada nuevo collector siga la interfaz definida.

---

## ADR-003: Uso de httplib en lugar de un framework HTTP pesado

**Estado:** Aceptado

### Contexto

ChuspaPulse expone métricas vía HTTP para integraciones con Prometheus, scripts y herramientas cliente. Se evaluaron opciones como Boost.Beast, Crow, Pistache y cpp-httplib.

### Decisión

Se usa **cpp-httplib** (header-only) como servidor HTTP embebido. No se usa ningún framework HTTP de mayor peso.

### Consecuencias

- ✅ Header-only: no requiere compilación separada ni linking adicional.
- ✅ Dependencia mínima — solo un archivo `.h` incluido en el proyecto.
- ✅ Suficiente para los endpoints actuales (`/metrics`, `/health`, `/config`, `/alerts`).
- ⚠️ No tiene soporte nativo para WebSockets ni HTTP/2.
- ⚠️ No escala a miles de conexiones concurrentes — adecuado para uso local o en red interna.

---

## ADR-004: TOML como formato de configuración

**Estado:** Aceptado

### Contexto

ChuspaPulse necesita un archivo de configuración para parámetros como el puerto HTTP, el intervalo de muestreo, la ruta de la base de datos y el nivel de log. Se evaluaron JSON, YAML, INI y TOML.

### Decisión

Se usa **TOML** como formato de configuración, leído desde `chuspapulse.toml` en el directorio de ejecución.

```toml
[servidor]
host = "0.0.0.0"
puerto = 8080

[sampler]
intervalo_segundos = 10

[storage]
ruta_db = "chuspapulse.db"
nivel_log = "info"
```

### Consecuencias

- ✅ Legible para humanos sin ambigüedades de indentación (a diferencia de YAML).
- ✅ Tipado explícito: strings entre comillas, enteros sin comillas, booleanos claros.
- ✅ Soporte de secciones (`[servidor]`, `[sampler]`) que organizan la configuración.
- ⚠️ Menos conocido que JSON o YAML para usuarios sin experiencia previa.

---

## ADR-005: Arquitectura fail-safe de collectors

**Estado:** Aceptado

### Contexto

Algunos collectors pueden fallar en ciertas plataformas o configuraciones (por ejemplo, el collector de batería en un servidor sin batería, o el collector de temperatura en un sistema sin sensores expuestos). Si un collector falla y no se maneja correctamente, puede interrumpir la recolección de todas las métricas.

### Decisión

Cada collector se ejecuta dentro de un bloque `try/catch`. Si un collector lanza una excepción, el error se registra en el log y el sistema **continúa con los demás collectors** sin interrumpir el ciclo de muestreo.

```cpp
for (auto& collector : collectors) {
    try {
        auto snapshots = collector->collect();
        // procesar snapshots
    } catch (const std::exception& e) {
        logger.warn("Collector '{}' falló: {}", collector->nombre(), e.what());
    }
}
```

### Consecuencias

- ✅ Un collector que falla no interrumpe a los demás — el sistema sigue funcionando.
- ✅ Los errores quedan registrados en el log para diagnóstico.
- ✅ Permite ejecutar ChuspaPulse en entornos donde no todos los collectors tienen soporte.
- ⚠️ Un collector que falla silenciosamente podría pasar desapercibido si no se revisan los logs.