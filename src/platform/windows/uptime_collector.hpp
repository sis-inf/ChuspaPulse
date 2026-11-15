#pragma once

#include "../../collectors/uptime/uptime_collector.h"

#ifdef _WIN32

namespace chuspapulse::platform::windows {

UptimeInfo getUptimeWindows();

}

#endif
