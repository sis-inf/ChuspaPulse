#include "gestor_alertas.hpp"

namespace pulso::alertas {

void GestorAlertas::addAlerta(const AlertaUmbral& alerta) {
    alertas_.push_back(alerta);
}

void GestorAlertas::setCallback(std::function<void(const std::string&)> callback) {
    callback_ = callback;
}

void GestorAlertas::addNotificador(std::shared_ptr<INotificador> notificador) {
    if (notificador) {
        notificadores_.push_back(notificador);
    }
}

void GestorAlertas::evaluar(const pulso::MetricSnapshot& snap) const {
    if (alertas_.empty()) {
        return;
    }

    for (const auto& alerta : alertas_) {
        if (alerta.evaluar(snap)) {
            std::string msg = alerta.mensaje();

            // Ejecutar callback si existe
            if (callback_) {
                callback_(msg);
            }

            // Invocar notificadores registrados
            for (const auto& notificador : notificadores_) {
                if (notificador) {
                    notificador->notificar(msg);
                }
            }
        }
    }
}

std::size_t GestorAlertas::cantidadAlertas() const {
    return alertas_.size();
}

} // namespace pulso::alertas