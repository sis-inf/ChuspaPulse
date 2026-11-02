#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <cstdint>
#include <vector>
#include <functional>
#include <string>
#include "alerta_umbral.hpp"

namespace chuspapulse::alertas {

/**
 * @brief Registro de una alerta disparada y guardada en el historial.
 */
struct RegistroAlerta {
    int id{0};
    std::int64_t timestamp{0};
    std::string metrica;
    double umbral{0.0};
    double valor{0.0};
    std::string mensaje;
};

/**
 * @brief Gestor de múltiples alertas de umbral con callback de notificación.
 *
 * Permite registrar varias AlertaUmbral y evaluarlas todas contra un
 * MetricSnapshot. Cuando una alerta se dispara, ejecuta un callback
 * con el mensaje descriptivo de la alerta y, si se configuró una base
 * de datos, la registra en la tabla `alerts`.
 */
class GestorAlertas {
public:
    GestorAlertas() = default;

    /**
     * @brief Construye un gestor que persiste las alertas disparadas.
     * @param db Base de datos SQLite con el esquema ya inicializado.
     */
    explicit GestorAlertas(SQLite::Database& db);

    ~GestorAlertas() = default;

    /**
     * @brief Registra una nueva alerta en el gestor.
     * @param alerta Instancia de AlertaUmbral a agregar.
     */
    void addAlerta(const AlertaUmbral& alerta);

    /**
     * @brief Establece el callback de notificación.
     * @param callback Función que recibe el mensaje de la alerta disparada.
     */
    void setCallback(std::function<void(const std::string&)> callback);

    /**
     * @brief Evalúa todas las alertas registradas contra el snapshot.
     *
     * Solo llama el callback para las alertas que se disparan.
     * Si no hay callback registrado, solo se omite la notificación: la alerta
     * se sigue guardando si hay una base de datos configurada.
     * Si no hay alertas registradas, no hace nada.
     * Si hay una base de datos configurada, cada alerta disparada se
     * inserta en la tabla `alerts`.
     *
     * @param snap Snapshot con las métricas actuales del sistema.
     */
    void evaluar(const chuspapulse::MetricSnapshot& snap) const;

    /**
     * @brief Devuelve la cantidad de alertas registradas.
     * @return Número de alertas en el gestor.
     */
    std::size_t cantidadAlertas() const;

    /**
     * @brief Devuelve las alertas registradas.
     */
    const std::vector<AlertaUmbral>& getAlertas() const;
     /**
     * @brief Devuelve el historial de alertas guardadas, de la más reciente
     *        a la más antigua (ordenado por timestamp).
     * @param limite Cantidad máxima de registros a devolver.
     * @param offset Cantidad de registros a omitir (paginación).
     * @return Registros de alertas; vacío si no hay base de datos configurada.
     */
    std::vector<RegistroAlerta> obtenerHistorial(std::size_t limite = 100,
                                                 std::size_t offset = 0) const;

private:
    SQLite::Database* db_{nullptr};
    std::vector<AlertaUmbral> alertas_;
    std::function<void(const std::string&)> callback_;
};

} // namespace chuspapulse::alertas
