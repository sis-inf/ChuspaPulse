# Guía para crear un nuevo endpoint HTTP en ChuspaPulse

Esta guía explica cómo agregar un nuevo endpoint HTTP en el proyecto ChuspaPulse siguiendo la arquitectura y convenciones actuales del código.

---

## 1. Convención de nombres de archivos

Los endpoints HTTP deben seguir la siguiente estructura:

```
src/http/handler_<nombre>.hpp
src/http/handler_<nombre>.cpp
```

Ejemplos existentes:

```
src/http/handler_health.hpp
src/http/handler_health.cpp

src/http/handler_history.hpp
src/http/handler_history.cpp

src/http/handler_prometheus.hpp
src/http/handler_prometheus.cpp
```

El archivo `.hpp` contiene la declaración de la función y el archivo `.cpp` contiene la implementación del comportamiento del endpoint.

---

# 2. Patrones utilizados para endpoints HTTP

ChuspaPulse utiliza dos patrones principales para implementar endpoints HTTP:

- Funciones `handle*` que generan directamente una respuesta.
- Funciones `registrar*` que registran una ruta dentro del servidor HTTP.

---

# 2.1 Patrón handle* (respuesta directa)

Este patrón se utiliza cuando el endpoint solamente necesita generar una respuesta simple y no requiere registrar lógica compleja.

Ejemplo existente:

```
GET /health
```

Archivos:

```
handler_health.hpp
handler_health.cpp
```

El header declara una función que retorna una cadena:

```cpp
std::string handleHealth(
    std::chrono::steady_clock::time_point start_time
);
```

La implementación genera una respuesta JSON:

```cpp
std::string handleHealth(
    std::chrono::steady_clock::time_point start_time)
{
    // Crear respuesta JSON

    return json;
}
```

Luego se conecta desde `main.cpp`:

```cpp
server.Get("/health",
    [&start_time](
        const httplib::Request&,
        httplib::Response& res)
{
    res.set_content(
        ChuspaPulse::http::handleHealth(start_time),
        "application/json"
    );
});
```

### ¿Cuándo utilizar handle*?

Se utiliza cuando:

- La respuesta es sencilla.
- No necesita consultar la base de datos.
- No requiere parámetros complejos.
- Solamente devuelve información generada en el momento.

Ejemplos:

- `/health`
- `/version`

---

# 2.2 Patrón registrar* (registro del endpoint)

Este patrón se utiliza cuando el endpoint necesita acceder a otros componentes del sistema o tiene una lógica más completa.

Ejemplo existente:

```
GET /metrics/history
```

Archivo:

```
handler_history.hpp
```

Declaración:

```cpp
void registrarHistory(
    httplib::Server& servidor,
    const ChuspaPulse::storage::Storage& storage,
    const ChuspaPulse::formatters::IFormatter& formatter
);
```

Implementación:

```cpp
void registrarHistory(
    httplib::Server& servidor,
    const ChuspaPulse::storage::Storage& storage,
    const ChuspaPulse::formatters::IFormatter& formatter)
{
    servidor.Get("/metrics/history",
        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            // Procesar petición
            // Consultar datos
            // Generar respuesta
        });
}
```

Luego se registra desde `main.cpp`:

```cpp
ChuspaPulse::http::registrarHistory(
    server,
    storage,
    formatterJson
);
```

### ¿Cuándo utilizar registrar*?

Se utiliza cuando:

- El endpoint necesita acceder a Storage.
- Tiene parámetros HTTP.
- Tiene lógica más extensa.
- Necesita recibir dependencias externas.

Ejemplos:

- `/metrics/history`
- `/metrics/prometheus`

---

# 3. Agregar el endpoint al sistema de compilación

Después de crear los archivos:

```
src/http/handler_nuevo.hpp
src/http/handler_nuevo.cpp
```

se debe agregar el archivo `.cpp` en:

```
src/CMakeLists.txt
```

Ejemplo:

```cmake
http/handler_nuevo.cpp
```

Dentro de:

```cmake
target_sources(ChuspaPulse PRIVATE
    ...
    http/handler_health.cpp
    http/handler_history.cpp
    http/handler_prometheus.cpp
    http/handler_nuevo.cpp
)
```

Esto permite que CMake incluya el nuevo código durante la compilación.

---

# 4. Crear el header del endpoint

Ejemplo:

Archivo:

```
src/http/handler_version.hpp
```

Contenido:

```cpp
#pragma once

#include <string>

namespace ChuspaPulse::http {

std::string handleVersion();

}
```

---

# 5. Implementar el endpoint

Archivo:

```
src/http/handler_version.cpp
```

Ejemplo:

```cpp
#include "handler_version.hpp"

namespace ChuspaPulse::http {

std::string handleVersion()
{
    return R"({
        "version":"0.1.0"
    })";
}

}
```

---

# 6. Incluir el handler en main.cpp

Agregar:

```cpp
#include "http/handler_version.hpp"
```

junto con los demás handlers:

```cpp
#include "http/handler_health.hpp"
#include "http/handler_history.hpp"
#include "http/handler_prometheus.hpp"
#include "http/handler_version.hpp"
```

---

# 7. Registrar el endpoint en main.cpp

Ejemplo de un endpoint simple:

```cpp
server.Get("/version",
    [](
        const httplib::Request&,
        httplib::Response& res)
{
    res.set_content(
        ChuspaPulse::http::handleVersion(),
        "application/json"
    );
});
```

Después de compilar estará disponible:

```
GET /version
```

---

# 8. Ejemplo completo: crear un endpoint ficticio

Supongamos que se necesita agregar:

```
GET /diagnostico
```

## Paso 1: Crear archivos

```
src/http/handler_diagnostico.hpp
src/http/handler_diagnostico.cpp
```

## Paso 2: Declarar la función

```cpp
std::string handleDiagnostico();
```

## Paso 3: Implementar la lógica

Crear la implementación necesaria para generar la respuesta del endpoint.

## Paso 4: Agregar a CMake

Modificar:

```
src/CMakeLists.txt
```

Añadiendo:

```cmake
http/handler_diagnostico.cpp
```

## Paso 5: Incluir en main.cpp

```cpp
#include "http/handler_diagnostico.hpp"
```

## Paso 6: Registrar la ruta

```cpp
server.Get("/diagnostico",
    [](
        const httplib::Request&,
        httplib::Response& res)
{
    res.set_content(
        ChuspaPulse::http::handleDiagnostico(),
        "application/json"
    );
});
```

## Paso 7: Compilar y probar

Compilar:

```bash
cmake -B build
cmake --build build
```

Probar:

```bash
curl http://localhost:8080/diagnostico
```

---

# Conclusión

Para agregar un nuevo endpoint HTTP en ChuspaPulse se deben seguir estos pasos:

1. Crear `handler_nombre.hpp` y `handler_nombre.cpp`.
2. Elegir el patrón adecuado:
   - `handle*` para respuestas simples.
   - `registrar*` para endpoints con lógica adicional.
3. Agregar el archivo al `CMakeLists.txt`.
4. Incluir el header en `main.cpp`.
5. Registrar la ruta HTTP.
6. Compilar y probar el endpoint.

Seguir esta estructura mantiene organizada la arquitectura HTTP de ChuspaPulse y facilita la incorporación de nuevos endpoints.
