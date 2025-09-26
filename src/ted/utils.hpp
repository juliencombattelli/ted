#ifndef TED_UTILS_HPP_
#define TED_UTILS_HPP_

#include <format>
#include <source_location>
#include <string_view>

#define TED_STRINGIFY_VALUE_OF(x) TED_STRINGIFY(x)
#define TED_STRINGIFY(x) #x

namespace ted::utils {

// Wrap a std::format_string and a default constructed std::source_location to
// get source location info for APIs using <format>
struct Fmt : public std::string_view {
    std::source_location source_location;

    // NOLINTNEXTLINE(*explicit*): intentionally allow implicit conversions
    template<typename String>
        requires std::constructible_from<std::string_view, String>
    consteval Fmt(
        const String& string,
        const std::source_location sourceLocation_
        = std::source_location::current()) noexcept
        : std::string_view(string)
        , source_location(sourceLocation_)
    {
    }

    template<std::formattable<char>... Args>
    [[nodiscard]] constexpr const std::format_string<Args...>& get(
        void) const noexcept
    {
        static_assert(
            sizeof(std::string_view) == sizeof(std::format_string<Args...>),
            "This implementation is not compatible with compiled STL");
        return reinterpret_cast<const std::format_string<Args...>&>(*this);
    }
};

} // namespace ted::utils

#endif // TED_UTILS_HPP_