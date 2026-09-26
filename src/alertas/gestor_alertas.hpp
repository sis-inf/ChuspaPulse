#pragma once

#include <vector>
#include <functional>
#include <string>
#include <memory>
#include "alerta_umbral.hpp"
#include "inotificador.hpp"

namespace pulso::alertas {

class GestorAlertas {
public:
    GestorAlertas() = default;
    ~GestorAlertas() = default;

    void addAlerta(const AlertaUmbral& alerta);
    void setCallback(std::function<void(const std::string&)> callback);

    /**
     * @brief Registra un notificador externo en el gestor.
     * @param notificador Instancia compartida de INotificador.
     */
    void addNotificador(std::shared_ptr<INotificador> notificador);

    void evaluar(const pulso::MetricSnapshot& snap) const;

    std::size_t cantidadAlertas() const;

private:
    std::vector<AlertaUmbral> alertas_;
    std::function<void(const std::string&)> callback_;
    std::vector<std::shared_ptr<INotificador>> notificadores_;
};

} // namespace pulso::alertas