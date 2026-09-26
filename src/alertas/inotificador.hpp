#pragma once

#include <string>

namespace pulso::alertas {

/**
 * @brief Interfaz para notificadores de alertas.
 */
class INotificador {
public:
    virtual ~INotificador() = default;

    /**
     * @brief Envía una notificación con el mensaje de la alerta.
     * @param mensaje Texto descriptivo de la alerta disparada.
     */
    virtual void notificar(const std::string& mensaje) = 0;
};

} // namespace pulso::alertas