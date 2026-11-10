#pragma once

#include <httplib.h>

#include "../storage/storage.hpp"
#include "../formatters/iformatter.hpp"

namespace chuspapulse::http {

void registrarHistory(
    httplib::Server& servidor,
    const chuspapulse::storage::Storage& storage,
    const chuspapulse::formatters::IFormatter& formatter);

} // namespace chuspapulse::http
