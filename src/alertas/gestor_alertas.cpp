#include "gestor_alertas.hpp"

#include <chrono>

namespace chuspapulse::alertas {

GestorAlertas::GestorAlertas(SQLite::Database& db) : db_(&db) {}

void GestorAlertas::addAlerta(const AlertaUmbral& alerta) {
    alertas_.push_back(alerta);
}

void GestorAlertas::setCallback(std::function<void(const std::string&)> callback) {
    callback_ = callback;
}

void GestorAlertas::evaluar(const chuspapulse::MetricSnapshot& snap) const {
    if (alertas_.empty()) {
        return;
    }

    for (const auto& alerta : alertas_) {
        if (!alerta.evaluar(snap)) {
            continue;
        }

        if (callback_) {
            callback_(alerta.mensaje());
        }

        if (db_) {
            try {
                const auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();

                SQLite::Statement query(*db_,
                    "INSERT INTO alerts (timestamp, metrica, umbral, valor, mensaje) "
                    "VALUES (?, ?, ?, ?, ?)"
                );
                query.bind(1, static_cast<std::int64_t>(timestamp));
                query.bind(2, alerta.metrica());
                query.bind(3, static_cast<double>(alerta.umbral()));
                query.bind(4, alerta.valorActual(snap));
                query.bind(5, alerta.mensaje());
                query.exec();
            } catch (const std::exception& e) {
                if (callback_) {
                    callback_(std::string("No se pudo guardar la alerta: ") + e.what());
                }
            }
        }
    }
}

std::size_t GestorAlertas::cantidadAlertas() const {
    return alertas_.size();
}

const std::vector<AlertaUmbral>& GestorAlertas::getAlertas() const {
    return alertas_;
}

std::vector<RegistroAlerta> GestorAlertas::obtenerHistorial(
    std::size_t limite, std::size_t offset) const {
    std::vector<RegistroAlerta> historial;
    if (!db_) {
        return historial;
    }

    try {
        SQLite::Statement query(*db_,
            "SELECT id, timestamp, metrica, umbral, valor, mensaje FROM alerts "
            "ORDER BY timestamp DESC, id DESC LIMIT ? OFFSET ?"
        );
        query.bind(1, static_cast<std::int64_t>(limite));
        query.bind(2, static_cast<std::int64_t>(offset));

        while (query.executeStep()) {
            RegistroAlerta reg;
            reg.id = query.getColumn(0).getInt();
            reg.timestamp = query.getColumn(1).getInt64();
            reg.metrica = query.getColumn(2).getText();
            reg.umbral = query.getColumn(3).getDouble();
            reg.valor = query.getColumn(4).getDouble();
            reg.mensaje = query.getColumn(5).getText();
            historial.push_back(reg);
        }
    } catch (const std::exception&) {
        // En caso de que la tabla aún no exista.
    }

    return historial;
}

} // namespace chuspapulse::alertas
