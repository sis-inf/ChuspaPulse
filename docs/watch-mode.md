# Modo Watch (Visualización en Vivo)

El modo interactivo `--watch` permite visualizar las métricas del sistema en la terminal mediante una tabla que se actualiza automáticamente.

Este modo está pensado para realizar monitoreo continuo sin necesidad de ejecutar el comando manualmente varias veces.

---

## Uso básico

Para iniciar el modo de visualización en vivo:

```bash
ChuspaPulse --watch
```

Al ejecutarse, el modo `--watch` muestra la información de las métricas actualizada en la terminal.

---

## Configurar el intervalo de actualización

El intervalo de actualización puede modificarse mediante la opción `--watch-interval`.

Ejemplo:

```bash
ChuspaPulse --watch --watch-interval 2000
```

El valor está expresado en milisegundos. En este ejemplo:

- `2000` equivale a 2 segundos.
- La tabla será actualizada cada 2 segundos.

Otro ejemplo:

```bash
ChuspaPulse --watch --watch-interval 5000
```

Actualiza la información cada 5 segundos.

---

## Opciones disponibles

| Opción | Descripción | Ejemplo |
|--------|-------------|---------|
| `--watch` | Activa el modo interactivo de visualización en vivo | `ChuspaPulse --watch` |
| `--watch-interval <ms>` | Define cada cuánto tiempo se actualiza la información | `ChuspaPulse --watch --watch-interval 2000` |

---

## Ejemplo de salida en terminal

Al utilizar el modo `--watch`, la información se muestra en una tabla que se actualiza periódicamente:

```text
+--------------------------------+
|    CHUSPAPULSE - WATCH MODE    |
+--------------------------------+
| Métrica        | Valor         |
+--------------------------------+
| CPU            | 23.5 %        |
| RAM            | 45.0 %        |
| Disco          | 60.0 %        |
+--------------------------------+

Actualización: cada 1000 ms
Presione Ctrl+C para salir
```

La tabla se refresca automáticamente utilizando el intervalo configurado.

---

## Ejemplos de uso

### Monitoreo con intervalo predeterminado

```bash
ChuspaPulse --watch
```

Utiliza el intervalo establecido por defecto.

---

### Monitoreo con actualización cada 3 segundos

```bash
ChuspaPulse --watch --watch-interval 3000
```

La información se actualiza cada 3000 milisegundos.

---

### Monitoreo con actualización rápida

```bash
ChuspaPulse --watch --watch-interval 500
```

Permite observar cambios con mayor frecuencia.

---

## Finalizar el modo Watch

Para terminar el modo interactivo correctamente se debe utilizar:

```text
Ctrl + C
```

Al recibir la señal de interrupción, el modo watch finaliza la ejecución y libera los recursos utilizados.

Ejemplo:

```text
^C
Finalizando modo watch...
ChuspaPulse detenido correctamente.
```

---

## Recomendaciones

- Utilizar intervalos adecuados según la necesidad de monitoreo.
- Valores demasiado pequeños pueden generar actualizaciones frecuentes y aumentar el consumo de recursos.
- Para finalizar la ejecución siempre se recomienda utilizar `Ctrl + C` en lugar de cerrar la terminal directamente.
