#include "notificador_webhook.hpp"
#include <thread>
#include <iostream>
#include <nlohmann/json.hpp>

namespace pulso::alertas {

NotificadorWebhook::NotificadorWebhook(std::string url)
    : url_(std::move(url)) {}

void NotificadorWebhook::notificar(const std::string& mensaje) {
    if (url_.empty()) {
        return;
    }

    nlohmann::json payload = {
        {"mensaje", mensaje}
    };

    std::string body = payload.dump();
    std::string targetUrl = url_;

    // Se ejecuta en un hilo secundario para no bloquear el hilo principal (sampler)
    std::thread([targetUrl, body]() {
        try {
            // AQUÍ: Realizar la petición HTTP POST usando la librería de red del proyecto
            // Ej. httplib::Client cli(targetUrl);
            // cli.Post("/", body, "application/json");
        } catch (const std::exception& e) {
            std::cerr << "Error enviando webhook: " << e.what() << std::endl;
        }
    }).detach();
}

} // namespace pulso::alertas