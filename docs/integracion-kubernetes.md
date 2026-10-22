# Integración de ChuspaPulse con Kubernetes

## Resumen

ChuspaPulse puede desplegarse en un clúster Kubernetes para monitorear cada nodo del sistema.

En Kubernetes **no se recomienda utilizar un Deployment** para este propósito, ya que un Deployment únicamente mantiene un número fijo de réplicas y no garantiza que exista una instancia ejecutándose en todos los nodos.

El patrón recomendado para herramientas de monitoreo de infraestructura es un **DaemonSet**, ya que Kubernetes ejecutará automáticamente una instancia de ChuspaPulse en cada nodo del clúster.

---

# ¿Por qué usar DaemonSet?

Un DaemonSet garantiza que:

- Cada nodo tenga exactamente una instancia de ChuspaPulse.
- Los nuevos nodos reciban automáticamente un agente.
- Si un nodo desaparece, también desaparecerá su agente.
- No es necesario administrar manualmente el número de réplicas.

Por este motivo es el mecanismo estándar utilizado por agentes de monitoreo.

---

# Persistencia de datos

ChuspaPulse utiliza SQLite para almacenar información local.

Para evitar perder la base de datos al reiniciar un Pod se recomienda utilizar un **PersistentVolumeClaim (PVC)**.

Ejemplo:

```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: ChuspaPulse-storage
spec:
  accessModes:
    - ReadWriteOnce
  resources:
    requests:
      storage: 5Gi
```

El DaemonSet puede montar este volumen dentro del contenedor.

---

# DaemonSet de ejemplo

El archivo completo se encuentra en:

```
examples/kubernetes/daemonset.yaml
```

Este manifiesto despliega una instancia de ChuspaPulse por nodo.

---

# Exposición de métricas

Si el clúster utiliza Prometheus Operator, puede utilizarse un **ServiceMonitor** para que Prometheus descubra automáticamente el servicio de ChuspaPulse.

El manifiesto completo se encuentra en:

```
examples/kubernetes/servicemonitor.yaml
```

---

# Flujo recomendado

```
Nodo Kubernetes
        │
        ▼
   DaemonSet ChuspaPulse
        │
        ▼
  Endpoint /metrics
        │
        ▼
  ServiceMonitor
        │
        ▼
    Prometheus
        │
        ▼
    Grafana
```

---

# Ventajas del enfoque

- Monitoreo automático de todos los nodos.
- Escalado automático cuando aumenta el tamaño del clúster.
- Integración sencilla con Prometheus Operator.
- Persistencia de datos mediante PersistentVolumeClaim.
- Arquitectura alineada con las buenas prácticas de Kubernetes.

---

# Recomendaciones

- Utilizar siempre DaemonSet para agentes de monitoreo.
- Mantener la base SQLite sobre un volumen persistente.
- Utilizar ServiceMonitor cuando Prometheus Operator esté disponible.
- Verificar que el endpoint `/metrics` sea accesible desde Prometheus.