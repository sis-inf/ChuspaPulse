#pragma once

#include "inotificador.hpp"
#include <string>

namespace pulso::alertas {

/**
 * @brief Notificador que envía alertas mediante peticiones HTTP POST a un webhook.
 */
class NotificadorWebhook : public INotificador {
public:
    explicit NotificadorWebhook(std::string url);
    ~NotificadorWebhook() override = default;

    void notificar(const std::string& mensaje) override;

private:
    std::string url_;
};

} // namespace pulso::alertas