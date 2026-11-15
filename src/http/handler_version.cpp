#include "handler_version.hpp"
#include "../core/version.hpp"

namespace chuspapulse {
namespace http {

    std::string handleVersion() {
        return "{\"version\": \"" + chuspapulse::APP_VERSION + "\"}";
    }

} // namespace http
} // namespace chuspapulse
