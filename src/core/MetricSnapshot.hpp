#pragma once

#include <chrono>

namespace chuspapulse {

struct MetricSnapshot {
    double cpu;
    double ram;
    double disk;
    double rx_bytes;
    double tx_bytes;
    std::chrono::system_clock::time_point timestamp;

    // Constructor por defecto
    MetricSnapshot()
        : cpu(0)
        , ram(0)
        , disk(0)
        , rx_bytes(0)
        , tx_bytes(0)
        , timestamp(std::chrono::system_clock::now()) {}

    // Constructor con valores
    MetricSnapshot(double cpu_val, double ram_val, double disk_val,
                   double rx_val, double tx_val)
        : cpu(cpu_val)
        , ram(ram_val)
        , disk(disk_val)
        , rx_bytes(rx_val)
        , tx_bytes(tx_val)
        , timestamp(std::chrono::system_clock::now()) {}

    // Verifica que todas las métricas sean mayores a 0
    bool isValid() const {
        return cpu > 0 && ram > 0 && disk > 0 && rx_bytes > 0 && tx_bytes > 0;
    }
};

} // namespace chuspapulse
