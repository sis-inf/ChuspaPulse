#include "uptime_collector.hpp"

#ifdef _WIN32

#include <cstdint>
#include <windows.h>

namespace chuspapulse::platform::windows {

UptimeInfo getUptimeWindows() {
    ULONGLONG milliseconds = GetTickCount64();

    long long total_seconds = milliseconds / 1000;

    UptimeInfo info{};
    info.days = static_cast<std::uint32_t>(total_seconds / 86400);
    total_seconds %= 86400;

    info.hours = static_cast<std::uint32_t>(total_seconds / 3600);
    total_seconds %= 3600;

    info.minutes = static_cast<std::uint32_t>(total_seconds / 60);
    info.seconds = static_cast<std::uint32_t>(total_seconds % 60);

    return info;
}

}

#endif
