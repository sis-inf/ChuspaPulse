#pragma once

#include <string>
#include <vector>

namespace chuspapulse {
    struct MetricSnapshot;
    namespace alertas { class GestorAlertas; }
}

namespace chuspapulse::http {

/**
 * @brief Datos de una alerta activa tal como se devuelven en GET /alerts.
 */
struct AlertaActiva {
    int id;
    std::string mensaje;
    std::string timestamp;
};

/**
 * @brief Serializa las alertas activas al JSON de GET /alerts.
 *
 * Los campos de texto se escapan con nlohmann::json, por lo que la salida
 * es JSON válido aunque contengan comillas, barras invertidas o
 * caracteres de control.
 */
std::string serializarAlertas(const std::vector<AlertaActiva>& alertas);

/**
 * @brief Genera el JSON de GET /alerts con las alertas activas para el snapshot dado.
 */
std::string handleAlerts(const chuspapulse::alertas::GestorAlertas& gestor,
                         const chuspapulse::MetricSnapshot& snap);

} // namespace chuspapulse::http
