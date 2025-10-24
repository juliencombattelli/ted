#ifndef TED_UTILS_STRONG_TYPE_HPP_
#define TED_UTILS_STRONG_TYPE_HPP_

// Based on:
// - https://github.com/rollbear/strong_type
// - https://github.com/anthonywilliams/strong_typedef

// Other interesting references:
// - https://github.com/foonathan/type_safe
// - https://github.com/joboccara/NamedType

#if __cplusplus < 202302L
#error C++23 or greater is required
#endif
#if __cpp_explicit_this_parameter < 202110L
#error C++ feature `deducing this` is required
#endif

#include <concepts>
#include <limits>
#include <utility>

namespace ted::utils {

template<typename TProperty, typename T>
using property = typename TProperty::template property<T>;

template<typename T, typename... TProperties>
struct strong_type : property<TProperties, strong_type<T, TProperties...>>... {
    using value_type = T;

    strong_type() = delete;

    explicit constexpr strong_type(value_type v) noexcept(
        std::is_nothrow_move_constructible_v<value_type>)
        : value_(std::move(v))
    {
    }

    template<class Self>
    constexpr auto&& value(this Self&& self)
    {
        return std::forward<Self>(self).value_;
    }

    value_type value_;
};

struct incrementable {
    template<typename TStrong>
    struct property {
        friend constexpr TStrong& operator++(TStrong& strong)
        {
            ++strong.value();
            return strong;
        }
        friend constexpr TStrong operator++(TStrong& strong, int)
        {
            auto copy = strong;
            ++strong;
            return copy;
        }
    };
};

struct decrementable {
    template<typename TStrong>
    struct property {
        friend constexpr TStrong& operator--(TStrong& strong)
        {
            --strong.value();
            return strong;
        }
        friend constexpr TStrong operator--(TStrong& strong, int)
        {
            auto copy = strong;
            --strong;
            return copy;
        }
    };
};

struct bicrementable {
    template<typename TStrong>
    struct property : incrementable::property<TStrong>,
                      decrementable::property<TStrong> { };
};

struct arithmetic {
    template<typename TStrong>
    class property {
        [[nodiscard]]
        friend constexpr TStrong operator-(const TStrong& lh)
        {
            return TStrong { -lh.value() };
        }

        friend constexpr TStrong& operator+=(TStrong& lh, const TStrong& rh)
        {
            lh.value() += rh.value();
            return lh;
        }

        friend constexpr TStrong& operator-=(TStrong& lh, const TStrong& rh)
        {
            lh.value() -= rh.value();
            return lh;
        }

        friend constexpr TStrong& operator*=(TStrong& lh, const TStrong& rh)
        {
            lh.value() *= rh.value();
            return lh;
        }

        friend constexpr TStrong& operator/=(TStrong& lh, const TStrong& rh)
        {
            lh.value() /= rh.value();
            return lh;
        }

        template<
            typename TT = TStrong,
            typename = decltype(std::declval<TT>() % std::declval<TT>())>
        friend constexpr TStrong& operator%=(TStrong& lh, const TStrong& rh)
        {
            lh.value() %= rh.value();
            return lh;
        }

        [[nodiscard]]
        friend constexpr TStrong operator+(TStrong lh, const TStrong& rh)
        {
            lh += rh;
            return lh;
        }

        [[nodiscard]]
        friend constexpr TStrong operator-(TStrong lh, const TStrong& rh)
        {
            lh -= rh;
            return lh;
        }

        [[nodiscard]]
        friend constexpr TStrong operator*(TStrong lh, const TStrong& rh)
        {
            lh *= rh;
            return lh;
        }

        [[nodiscard]]
        friend constexpr TStrong operator/(TStrong lh, const TStrong& rh)
        {
            lh /= rh;
            return lh;
        }

        template<
            typename TT = TStrong,
            typename = decltype(std::declval<TT>() % std::declval<TT>())>
        [[nodiscard]] friend constexpr TStrong operator%(
            TStrong lh,
            const TStrong& rh)
        {
            lh %= rh;
            return lh;
        }
    };
};

} // namespace ted::utils

template<typename T, typename Tag, typename... Ms>
// TODO restore requires expression below
// requires strong::type_is_v<strong_type<T, Tag, Ms...>, strong::arithmetic>
class std::numeric_limits<ted::utils::strong_type<T, Tag, Ms...>>
    : public std::numeric_limits<T> {
    using type = ted::utils::strong_type<T, Tag, Ms...>;

public:
    [[nodiscard]] static constexpr type min() noexcept
    {
        return type { std::numeric_limits<T>::min() };
    }
    [[nodiscard]] static constexpr type lowest() noexcept
    {
        return type { std::numeric_limits<T>::lowest() };
    }
    [[nodiscard]] static constexpr type max() noexcept
    {
        return type { std::numeric_limits<T>::max() };
    }
    [[nodiscard]] static constexpr type epsilon() noexcept
    {
        return type { std::numeric_limits<T>::epsilon() };
    }
    [[nodiscard]] static constexpr type round_error() noexcept
    {
        return type { std::numeric_limits<T>::round_error() };
    }
    [[nodiscard]] static constexpr type infinity() noexcept
    {
        return type { std::numeric_limits<T>::infinity() };
    }
    [[nodiscard]] static constexpr type quiet_NaN() noexcept
    {
        return type { std::numeric_limits<T>::quiet_NaN() };
    }
    [[nodiscard]] static constexpr type signaling_NaN() noexcept
    {
        return type { std::numeric_limits<T>::signaling_NaN() };
    }
    [[nodiscard]] static constexpr type denorm_min() noexcept
    {
        return type { std::numeric_limits<T>::denorm_min() };
    }
};

#endif // TED_UTILS_STRONG_TYPE_HPP_