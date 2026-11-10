# Plan de Pruebas

## \## 1. Objetivo

## Verificar que el sistema funcione correctamente, asegurando que los módulos de monitoreo y almacenamiento trabajen sin errores.

## \## 2. Alcance

## \* \*\*En alcance\*\*: Módulo de base de datos SQLite, proceso de empaquetado `npm pack` y documentación técnica.

## \* \*\*Fuera de alcance\*\*: Interfaz gráfica de usuario y despliegue en servidores externos.

## \## 3. Tipos de prueba

## \* \[x] \*\*Unitarias\*\*: Para verificar las funciones de la base de datos.

## \* \[x] \*\*Integración\*\*: Para asegurar que el almacenamiento guarda bien las capturas.

## \* \[x] \*\*Funcionales\*\*: Para validar que el paquete final no incluya archivos basura.

## \## 4. Entornos

## 

## | Entorno | SO | Versión |C

## | :--- | :--- | :--- |

## | Local | Windows 11 | Node.js v20+ |

## | CI | Ubuntu latest | GitHub Actions |

## 

## \## 5. Responsables

## 

## | Rol | Responsable |

## | :--- | :--- |

## | Diseño de casos | Carlos Gustavo Choquecallata |

## | Ejecución manual | Carlos Gustavo Choquecallata |

## | Automatización | GitHub Actions |

## 

## \## 6. Criterios de salida

## \* \[x] Cobertura mínima de los módulos principales.

## \* \[x] Cero bugs críticos abiertos en la persistencia de datos.

## \* \[x] Todos los casos de prueba ejecutados satisfactoriamente.

## \## 7. Riesgos

## 

## | Riesgo | Probabilidad | Impacto | Mitigación |

## | :--- | :--- | :--- | :--- |

## | Corrupción de base de datos | Baja | Alto | Usar scripts de inicialización limpios |

## \## 8. Módulo de alertas (AlertaUmbral y GestorAlertas)

## \### 8.1 AlertaUmbral

## \* \*\*Funciones a probar\*\*: constructor, `evaluar()`, `mensaje()`.

## \* \*\*Casos de prueba recomendados\*\*:

##   \* \[x] Operador `>` dispara cuando la métrica supera el umbral.

##   \* \[x] Operador `>` no dispara cuando la métrica está por debajo del umbral.

##   \* \[x] Operador `<` dispara cuando la métrica es inferior al umbral.

##   \* \[x] Operador `<` no dispara cuando la métrica es igual o superior al umbral.

##   \* \[x] El constructor lanza `std::invalid_argument` si el operador no es `'>'` ni `'<'`.

##   \* \[x] `evaluar()` lanza `std::invalid_argument` si la métrica no existe en el snapshot.

##   \* \[x] `mensaje()` retorna una cadena no vacía con el nombre de la métrica, el operador y el umbral.

##   \* \[x] Comportamiento en el valor límite exacto (métrica == umbral) para ambos operadores.

## \### 8.2 GestorAlertas

## \* \*\*Funciones a probar\*\*: `addAlerta()`, `setCallback()`, `evaluar()`, `cantidadAlertas()`.

## \* \*\*Casos de prueba recomendados\*\*:

##   \* \[x] El callback se llama una vez cuando una alerta registrada se dispara.

##   \* \[x] El callback no se llama cuando ninguna alerta se dispara.

##   \* \[x] El callback se llama una vez por cada alerta activa cuando hay varias alertas registradas.

##   \* \[x] `evaluar()` no falla si no hay alertas registradas.

##   \* \[x] `evaluar()` no falla ni llama al callback si no se ha configurado ninguno.

##   \* \[x] `cantidadAlertas()` refleja correctamente el número de alertas agregadas.

## \## 9. Módulo de formatters (FormatterCSV y FormatterText)

## \### 9.1 FormatterCSV

## \* \*\*Funciones a probar\*\*: `formato()`, `contentType()`, `formatear()`, `formatearHistorial()`.

## \* \*\*Casos de prueba recomendados\*\*:

##   \* \[x] `formato()` retorna `"csv"` y `contentType()` retorna `"text/csv"`.

##   \* \[x] La primera llamada a `formatear()` incluye la línea de cabecera.

##   \* \[x] Llamadas subsecuentes a `formatear()` sobre la misma instancia no repiten la cabecera.

##   \* \[x] `formatearHistorial()` siempre incluye la cabecera, sin importar llamadas previas a `formatear()`.

##   \* \[x] `formatearHistorial()` con una lista vacía de snapshots retorna solo la cabecera.

##   \* \[x] Los valores numéricos se formatean con 2 decimales.

##   \* \[x] La conversión de unidades es correcta (bytes a MB para RAM, bytes a GB para disco, bytes a KB para red).

##   \* \[x] Métricas ausentes en el snapshot se serializan como `0.00` sin romper el formato CSV.

## \### 9.2 FormatterText

## \* \*\*Funciones a probar\*\*: `formato()`, `contentType()`, `formatear()`, `formatearHistorial()`.

## \* \*\*Casos de prueba recomendados\*\*:

##   \* \[x] `formato()` retorna `"text"` y `contentType()` retorna `"text/plain"`.

##   \* \[x] `formatear()` produce una línea con el orden esperado: timestamp, CPU%, RAM usada/total, Disco usado/total, Red rx/tx.

##   \* \[x] El timestamp se convierte correctamente a formato ISO 8601 (UTC).

##   \* \[x] Se reconocen ambos alias de nombre de métrica (ej. `"cpu.usage"` y `"cpu"`, `"ram.used"` y `"ram.usado"`).

##   \* \[x] `formatearHistorial()` con múltiples snapshots genera una línea por snapshot, separadas por salto de línea.

##   \* \[x] `formatearHistorial()` con una lista vacía retorna una cadena vacía.

##   \* \[x] El porcentaje de CPU se muestra con 1 decimal.

## \## 10. Módulo de HTTP handlers (handler\_health y handler\_config)

## \### 10.1 handler\_health (GET /health)

## \* \*\*Funciones a probar\*\*: `handleHealth()`.

## \* \*\*Casos de prueba recomendados\*\*:

##   \* \[x] La respuesta es un JSON válido con las claves `status`, `uptime_seconds`, `version` y `hostname`.

##   \* \[x] `status` es siempre `"ok"`.

##   \* \[x] `uptime_seconds` aumenta de forma consistente al avanzar el tiempo desde `start_time`.

##   \* \[x] `version` corresponde al valor hardcodeado `"0.1.0"`.

##   \* \[x] `hostname` no está vacío y corresponde al nombre real de la máquina.

##   \* \[x] Si `start_time` es el instante actual, `uptime_seconds` es `0` o un valor cercano a `0`.

## \### 10.2 handler\_config (GET /config)

## \* \*\*Funciones a probar\*\*: `handleConfig()`.

## \* \*\*Casos de prueba recomendados\*\*:

##   \* \[x] La respuesta es un JSON válido con las claves `interval_ms`, `http_port`, `log_level`, `output_format` y `version`.

##   \* \[x] `interval_ms` corresponde a `sampler.intervalo_segundos * 1000`.

##   \* \[x] `http_port` corresponde al puerto configurado en `servidor.puerto`.

##   \* \[x] Los campos de texto (`log_level`, `output_format`) se escapan correctamente si contienen comillas o backslashes.

##   \* \[x] La respuesta no expone campos sensibles o internos de `Config` que no estén en la lista pública (ej. `storage.ruta_db`, `servidor.host`).

##   \* \[x] Con una configuración por defecto (`ChuspaPulse::config::porDefecto()`), el JSON resultante coincide con los valores esperados.

