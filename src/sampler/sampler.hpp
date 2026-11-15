#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "../collectors/icollector.hpp"
#include "../core/types.hpp"
#include "../storage/storage.hpp"

namespace chuspapulse::sampler {

class Sampler {
public:
    /// Función que se invoca en cada ciclo con el snapshot recién recolectado.
    using SnapshotCallback = std::function<void(const chuspapulse::core::Snapshot&)>;

    Sampler(
        std::vector<std::shared_ptr<chuspapulse::collectors::ICollector>> collectors,
        chuspapulse::storage::Storage& storage,
        int intervaloMs,
        SnapshotCallback onSnapshot = nullptr);

    ~Sampler();

    /// Inicia el thread de muestreo. No bloquea.
    void iniciar();

    /// Solicita la detención y espera a que el thread termine.
    void detener();

    /// Devuelve true si el sampler está corriendo.
    bool corriendo() const;

private:
    std::vector<std::shared_ptr<chuspapulse::collectors::ICollector>> collectors_;
    chuspapulse::storage::Storage& storage_;
    /// Intervalo entre muestras, en milisegundos.
    int intervaloMs_;
    SnapshotCallback onSnapshot_;
    std::thread thread_;
    std::atomic<bool> corriendo_{false};
    std::condition_variable cv_;
    std::mutex mutex_;
};

} // namespace chuspapulse::sampler
