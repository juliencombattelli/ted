#include "ted/utils/strong_type.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <type_traits>

TEST(strong_type, arithmetic)
{
    using namespace ted::utils;

    using i32 = strong_type<int32_t, arithmetic>;

    static constexpr int32_t a = 2;
    static constexpr int32_t b = 42;

    i32 i { a };
    EXPECT_EQ(i.value(), a);
    i32 j(b);
    EXPECT_EQ(j.value(), b);
    i += j;
    EXPECT_EQ(i.value(), a + b);
    i -= j;
    EXPECT_EQ(i.value(), a);
    i *= j;
    EXPECT_EQ(i.value(), a * b);
    i /= j;
    EXPECT_EQ(i.value(), a);
    i = -j;
    EXPECT_EQ(i.value(), -b);
}
