#include "ted/editor.hpp"

#include <gtest/gtest.h>

#include <string>
#include <string_view>

static void gtest_validate(
    const ted::editor::SubstrResult& result,
    const ted::editor::SubstrResult& expected)
{
    EXPECT_EQ(result.substr, expected.substr);
    EXPECT_EQ(result.offset_from_start_char, expected.offset_from_start_char);
    EXPECT_EQ(result.cut_at_start, expected.cut_at_start);
    EXPECT_EQ(result.cut_at_end, expected.cut_at_end);
}

TEST(Utf8, reference_rendered_column_substr)
{
    const std::string str = "start\t\t\t\t\t\tend";

    ted::editor::SubstrResult expected_result {
        .substr = "\t\t",
        .offset_from_start_char = 2,
        .cut_at_start = 2,
        .cut_at_end = 3,
    };

    auto result = ted::editor::rendered_column_substr(str, 7, 13);
    gtest_validate(result, expected_result);
}

TEST(Utf8, rendered_column_count)
{
    const std::string str = "a👨‍🏭b👨‍🏭\tc\t👨d";

    static constexpr size_t expected_column_count = 18;

    static constexpr ted::editor::SubstrResult expected_result_substr_len[] {
        {
            .substr = "",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 2,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 3,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\t",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 2,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 3,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t👨",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t👨d",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
    };

    static constexpr ted::editor::SubstrResult
        expected_result_substr_at_start[] {
            {
                .substr = "a👨‍🏭b👨‍🏭\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨‍🏭b👨‍🏭\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "b👨‍🏭\tc\t👨d",
                .offset_from_start_char = 1,
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "b👨‍🏭\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨‍🏭\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "\tc\t👨d",
                .offset_from_start_char = 1,
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .offset_from_start_char = 1,
                .cut_at_start = 3,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .offset_from_start_char = 2,
                .cut_at_start = 2,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .offset_from_start_char = 3,
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .offset_from_start_char = 1,
                .cut_at_start = 3,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .offset_from_start_char = 2,
                .cut_at_start = 2,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .offset_from_start_char = 3,
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "d",
                .offset_from_start_char = 1,
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
        };

    static_assert(
        std::size(expected_result_substr_len) == expected_column_count + 1);
    static_assert(
        std::size(expected_result_substr_at_start)
        == expected_column_count + 1);

    size_t column_count = ted::editor::rendered_column_count(str);

    ASSERT_EQ(column_count, expected_column_count);

    column_count = std::min(column_count, expected_column_count);

    for (size_t len = 0; len <= column_count; len++) {
        auto result = ted::editor::rendered_column_substr(str, 0, len);

        gtest_validate(result, expected_result_substr_len[len]);
    }

    for (size_t start = 0; start <= column_count; start++) {
        auto result = ted::editor::rendered_column_substr(
            str,
            start,
            column_count - start);
        gtest_validate(result, expected_result_substr_at_start[start]);
    }
}

TEST(Utf8, column_substr)
{
    const std::string str = "a👨‍🏭b👨‍🏭\tc\t👨d";

    static constexpr size_t expected_column_count = 12;

    static constexpr ted::editor::SubstrResult expected_result_substr_len[] {
        {
            .substr = "",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\t",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t👨",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t👨d",
            .offset_from_start_char = 0,
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
    };

    static constexpr ted::editor::SubstrResult
        expected_result_substr_at_start[] {
            {
                .substr = "a👨‍🏭b👨‍🏭\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨‍🏭b👨‍🏭\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "b👨‍🏭\tc\t👨d",
                .offset_from_start_char = 1,
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "b👨‍🏭\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨‍🏭\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "\tc\t👨d",
                .offset_from_start_char = 1,
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "\tc\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "\t👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "d",
                .offset_from_start_char = 1,
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "d",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "",
                .offset_from_start_char = 0,
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
        };

    static_assert(
        std::size(expected_result_substr_len) == expected_column_count + 1);
    static_assert(
        std::size(expected_result_substr_at_start)
        == expected_column_count + 1);

    size_t column_count = ted::editor::column_count(str);

    ASSERT_EQ(column_count, expected_column_count);

    column_count = std::min(column_count, expected_column_count);

    for (size_t len = 0; len <= column_count; len++) {
        auto result = ted::editor::column_substr(str, 0, len);
        gtest_validate(result, expected_result_substr_len[len]);
    }

    for (size_t start = 0; start <= column_count; start++) {
        auto result
            = ted::editor::column_substr(str, start, column_count - start);
        gtest_validate(result, expected_result_substr_at_start[start]);
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    ted::editor::init();

    int result = RUN_ALL_TESTS();

    ted::editor::deinit();

    return result;
}
