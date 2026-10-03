## Evidencias de QA
En esta carpeta se deben guardar todas las evidencias (capturas de pantalla, logs, reportes) generadas durante la ejecución del checklist de QA para cada release.

Convención de Nombres y Carpetas
Para mantener el orden, las evidencias deben organizarse por versión de release y plataforma:

evidencias/
├── v1.0.0/
│ ├── linux/
│ │ ├── smoke_test_json.png
│ │ └── alertas.log
│ ├── windows/
│ └── macos/
└── v1.1.0/
└── linux/

- Los logs deben ir en formato `.txt` o `.log`.
- Las capturas de pantalla en `.png` o `.jpg`.
- Nombrar los archivos indicando la prueba realizada (ej. `http_endpoint_test.png`).