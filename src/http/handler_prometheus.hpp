#pragma once

#include <httplib.h>
#include "storage/storage.hpp"

namespace chuspapulse::http {

/// Registra el handler de /metrics/prometheus en el servidor httplib.
void registrarPrometheus(
    httplib::Server& servidor,
    const chuspapulse::storage::Storage& storage);

} // namespace chuspapulse::http