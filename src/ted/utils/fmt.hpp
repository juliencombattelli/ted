#ifndef TED_UTILS_FMT_HPP_
#define TED_UTILS_FMT_HPP_

#include <format>
#include <source_location>

namespace ted::utils {

// Wrap a std::format_string and a default constructed std::source_location to
// get source location info for APIs using <format>
// From https://stackoverflow.com/a/79155817/13195557
namespace impl {
template<class... Args>
struct Fmt {
    template<class T>
        requires std::constructible_from<std::format_string<Args...>, T const&>
    // NOLINTNEXTLINE(*explicit*): intentionally allow implicit conversions
    consteval Fmt(
        T const& fmt,
        std::source_location loc = std::source_location::current())
        : format(fmt)
        , location(loc)
    {
    }

    std::format_string<Args...> format;
    std::source_location location;
};
} // namespace impl

template<typename... Args>
using Fmt = impl::Fmt<std::type_identity_t<Args>...>;

} // namespace ted::utils

#endif // TED_UTILS_FMT_HPP_
