#include <ted/os.hpp>
#include <ted/term.hpp>

#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <vector>

namespace ted::os {

static void at_exit_handler();

static constexpr size_t AtExitRingCount = std::to_underlying(Ring::Count);

struct AtExit {
    AtExit();

    // TODO add mutex once we go multithreaded
    std::array<std::vector<void (*)()>, AtExitRingCount> exit_handlers;
};

struct AtExitRegisterer {
    AtExitRegisterer()
    {
        // Register the exit handler that will be called by the C++ runtime
        if (std::atexit(at_exit_handler) != 0) {
            (void)std::fputs("Cannot register atexit() handler", stderr);
            std::exit(EXIT_FAILURE);
        }
    }
};

// The definition order here matters
// First define `at_exit_registry` holding the list of handlers
// Then define `at_exit_registerer` registering the std::atexit handler
// This way it is guaranteed that on termination the handler will be called
// before the `at_exit_registry` destructor
static AtExit at_exit_registry;
static AtExitRegisterer at_exit_registerer;

static void at_exit_handler()
{
    for (size_t ring_ip1 = AtExitRingCount; ring_ip1 > 0; ring_ip1--) {
        const auto& ring = at_exit_registry.exit_handlers[ring_ip1 - 1];
        for (size_t handler_ip1 = ring.size(); handler_ip1 > 0; handler_ip1--) {
            const auto& handler = ring[handler_ip1 - 1];
            if (handler != nullptr) {
                handler();
            }
        }
    }
}

AtExit::AtExit()
{
    // Preallocate a reasonable amount of handlers in each ring
    for (auto& ring : at_exit_registry.exit_handlers) {
        ring.reserve(8);
    }
}

void at_exit(Ring ring, void (*handler)())
{
    if (ring == Ring::Reserved_0) {
        (void)std::fprintf(
            stderr,
            "AtExit::register_handler(): ring 0 is reserved\n");
        std::exit(EXIT_FAILURE);
    }
    if (ring >= Ring::Count) {
        (void)std::fprintf(
            stderr,
            "AtExit::register_handler(): cannot register in ring %d, maximum "
            "is %zu\n",
            std::to_underlying(ring),
            AtExitRingCount - 1);
        std::exit(EXIT_FAILURE);
    }
    at_exit_registry.exit_handlers[std::to_underlying(ring)].push_back(handler);
}

static void at_exit_ring0(void (*handler)())
{
    at_exit_registry.exit_handlers[0].push_back(handler);
}

void exit_ok()
{
    std::exit(EXIT_SUCCESS);
}

void exit_err(const char* msg)
{
    static const char* exit_message = msg;
    at_exit_ring0([] { std::perror(exit_message); });
    std::exit(EXIT_FAILURE);
}

} // namespace ted::os
