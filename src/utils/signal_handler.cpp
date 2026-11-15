#include <atomic>
#include <csignal>

static_assert(std::atomic<bool>::is_always_lock_free,
              "Signal flags require lock-free atomic<bool>.");

std::atomic<bool> isRunning{true};
std::atomic<bool> g_shutdown_requested{false};
std::atomic<bool> g_reload_requested{false};

void handler(int) {
  g_shutdown_requested.store(true, std::memory_order_relaxed);
  isRunning.store(false, std::memory_order_relaxed);
}

// Solo marca la solicitud: la recarga se hace fuera del handler.
void reloadHandler(int) {
  g_reload_requested.store(true, std::memory_order_relaxed);
}

bool is_shutdown_requested() {
  return g_shutdown_requested.load(std::memory_order_relaxed);
}

// Devuelve true si hay una recarga pendiente y la marca como atendida.
bool consume_reload_request() {
  return g_reload_requested.exchange(false, std::memory_order_relaxed);
}

void setupSignalHandler() {
  std::signal(SIGINT, handler);
  std::signal(SIGTERM, handler);
#ifdef SIGHUP
  std::signal(SIGHUP, reloadHandler);
#endif
}
