# Changelog Sprint 2

## Período del sprint

Segundo sprint de desarrollo del proyecto ChuspaPulse.

## Contribuidores activos

| Contribuidor | PRs mergeados |
|---|---|
| mary13761 | #495, #496, #497, #500 |
| Emerick99 | #503, #504 |
| danielamejia-101 | #485, #486, #487, #488 |
| Luza25-A | #497, #498 |
| YORDY-SG | #501, #506 |
| jhaf7712 | #508 |
| Nailea615 | #507 |
| claudiaatanacio-cloud | #502 |
| JuanPinaya | #485 |

## Módulos completados

| Módulo | Descripción | PR de referencia |
|---|---|---|
| storage | Rotación automática de snapshots | #504 |
| formatters/formatter_json | Tests unitarios de FormatterJSON | #503 |
| cmake | Registro de handler_metrics.cpp en target_sources | #507 |
| CI/valgrind | Detección de memory leaks en tests | #506 |
| .github/CODEOWNERS | Revisores por módulo | #501 |
| docs/troubleshooting | Guía de troubleshooting | #498 |
| docs/flujo-datos | Actualización con pipeline de alertas | #497 |
| docs/seguridad | Modelo de amenazas del servidor HTTP | #495 |
| docs/diseno-clases | Clases y módulos nuevos | #494 |
| docs/compilacion-linux | Ubuntu 24.04 y clang | #491 |
| docs/glosario | Términos de Linux y monitoreo | #487 |
| docs/instalacion | Ubuntu 22.04/24.04 y troubleshooting | #486 |
| docs/estructura-proyecto | Documentación de estructura | #488 |
| docs/tipos-datos | Tipos de datos del proyecto | #485 |
| README | CI badge e instrucciones de compilación | #484 |
| docs/plan-pruebas | Plan de pruebas para alertas, formatters y HTTP handlers | #508 |

## Issues cerradas

| Issue | Descripción |
|---|---|
| #380 | Plan de pruebas para alertas, formatters y HTTP handlers |
| #385 | Flujo de datos con pipeline de alertas |
| #382 | Diseño de clases con módulos nuevos |
| #381 | Casos de prueba funcionales |
| #374 | Modelo de amenazas del servidor HTTP |
| #372 | Compilacion-linux con Ubuntu 24.04 y clang |
| #370 | Glosario con términos de Linux y monitoreo |
| #368 | Instalacion con Ubuntu 22.04/24.04 y troubleshooting |
| #348 | Guía de troubleshooting |

## Métricas del sprint

| Métrica | Valor |
|---|---|
| Pull Requests mergeados | 16 |
| Issues cerradas | 9 |
| Contribuidores activos | 9 |
| Módulos con nuevas funcionalidades | 2 (storage, formatters) |
| Módulos de infraestructura | 2 (CI/valgrind, CODEOWNERS) |
| Documentos creados o actualizados | 10 |

## Retrospectiva del sprint

**Lo que funcionó bien:**
- Alta participación del equipo con 9 contribuidores activos.
- Cobertura amplia de documentación técnica.
- Implementación de rotación automática en storage.
- Configuración de revisores por módulo con CODEOWNERS.
- Integración de detección de memory leaks en CI.

**Lo que debe mejorar:**
- La estructura de directorios del proyecto no cumple estándares según el lenguaje, lo que causó el rechazo de varios PRs con código fuente.
- Varios PRs fueron enviados contra main en lugar de dev.
- La compilación del proyecto presenta problemas que bloquean los tests automatizados.
