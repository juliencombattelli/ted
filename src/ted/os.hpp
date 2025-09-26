#ifndef TED_OS_HPP_
#define TED_OS_HPP_

#include <ted/utils.hpp>

#include <cstdio>
#include <cstdlib>
#include <format>
#include <source_location>

namespace ted::os {

// Functions executed on normal program termination.
// Those functions are called in reverse order regarding when they were
// registered, in the same manner as std::atexit(). This mechanism also
// implements several rings where handlers are executed from the last ring to
// ring 0. This is particularly useful to exit from the TUI mode of Ted while
// properly restoring the terminal state, but still being able to print some
// error message visible to the user.
// Ring 0 is reserved for exit functions is this module.
//
// TODO if only ring 1 is needed in the other modules then consider keeping only
// two rings (one reserved to os::exit* and one public) to simplify usage
enum class Ring {
    Reserved_0 = 0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    Count,
};
void at_exit(Ring ring, void (*handler)());

// Terminate the program with a success status
[[noreturn]]
void exit_ok(std::source_location srcloc = std::source_location::current());

// Display an error message and terminate the program with a failure status
[[noreturn]]
void exit_err(
    const char* msg,
    std::source_location srcloc = std::source_location::current());

// Display a formatted error message and terminate the program with a failure
// status
template<class... Args>
[[noreturn]]
void exit_err_format(utils::Fmt fmt, Args&&... args)
{
    std::string msg
        = std::format(fmt.get<Args...>(), std::forward<Args>(args)...);
    exit_err(msg.c_str(), fmt.source_location);
}

// Prepend the source location before the exit message
void print_source_location_at_exit(bool do_print_source_location);

[[nodiscard]]
bool isatty(FILE* stream);

} // namespace ted::os

#endif // TED_OS_HPP_
