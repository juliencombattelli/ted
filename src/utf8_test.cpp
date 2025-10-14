#include "ted/editor.hpp"

#include <iostream>
#include <string>
#include <string_view>

static bool validate(
    const ted::editor::SubstrResult& result,
    const ted::editor::SubstrResult& expected)
{
    bool ok = true;
    if (result.substr != expected.substr) {
        ok = false;
        std::cout << "  error: expecting substr '" << expected.substr
                  << "', got '" << result.substr << "'\n";
    }
    if (result.cut_at_start != expected.cut_at_start) {
        ok = false;
        std::cout << "  error: expecting cut_at_start " << expected.cut_at_start
                  << ", got " << result.cut_at_start << "\n";
    }
    if (result.cut_at_end != expected.cut_at_end) {
        ok = false;
        std::cout << "  error: expecting cut_at_end " << expected.cut_at_end
                  << ", got " << result.cut_at_end << "\n";
    }
    return ok;
}

static bool run_test_non_rendered()
{
    const std::string str = "a👨‍🏭b👨‍🏭\tc\t👨d";

    static constexpr size_t expected_column_count = 12;

    static constexpr ted::editor::SubstrResult expected_result_substr_len[] {
        {
            .substr = "",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a",
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b",
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\t",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t",
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t👨",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t👨d",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
    };

    static constexpr ted::editor::SubstrResult
        expected_result_substr_at_start[] {
            {
                .substr = "a👨‍🏭b👨‍🏭\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨‍🏭b👨‍🏭\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "b👨‍🏭\tc\t👨d",
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "b👨‍🏭\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨‍🏭\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "\tc\t👨d",
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "d",
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
        };

    static_assert(
        std::size(expected_result_substr_len) == expected_column_count + 1);
    static_assert(
        std::size(expected_result_substr_at_start)
        == expected_column_count + 1);

    bool ok = true;

    size_t column_count = ted::editor::column_count(str);

    if (column_count != expected_column_count) {
        std::cout << "  error: expecting column_count " << expected_column_count
                  << ", got " << column_count << "\n";
        ok = false;
    }

    column_count = std::min(column_count, expected_column_count);

    std::cout << "Substringing from column 0 to column " << column_count
              << "\n";
    for (size_t len = 0; len <= column_count; len++) {
        auto result = ted::editor::column_substr(str, 0, len);
        std::cout << "str.substr(0, len=" << len << ")\n";
        std::cout << "  result is '" << result.substr << "'\n";
        ok = validate(result, expected_result_substr_len[len]) && ok;
    }

    std::cout << "Substringing from column " << column_count
              << " to column 0\n";
    for (size_t start = 0; start <= column_count; start++) {
        auto result
            = ted::editor::column_substr(str, start, column_count - start);
        std::cout << "str.substr(start=" << start
                  << ", len=" << column_count - start << ")\n";
        std::cout << "  result is '" << result.substr << "'\n";
        ok = validate(result, expected_result_substr_at_start[start]) && ok;
    }

    return ok;
}

static bool run_test_rendered()
{
    const std::string str = "a👨‍🏭b👨‍🏭\tc\t👨d";

    static constexpr size_t expected_column_count = 18;

    static constexpr ted::editor::SubstrResult expected_result_substr_len[] {
        {
            .substr = "",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a",
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b",
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .cut_at_start = 0,
            .cut_at_end = 2,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .cut_at_start = 0,
            .cut_at_end = 3,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\t",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .cut_at_start = 0,
            .cut_at_end = 2,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc",
            .cut_at_start = 0,
            .cut_at_end = 3,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t",
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t👨",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭\tc\t👨d",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
    };

    static constexpr ted::editor::SubstrResult
        expected_result_substr_at_start[] {
            {
                .substr = "a👨‍🏭b👨‍🏭\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨‍🏭b👨‍🏭\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "b👨‍🏭\tc\t👨d",
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "b👨‍🏭\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨‍🏭\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "\tc\t👨d",
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "\tc\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .cut_at_start = 3,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .cut_at_start = 2,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "c\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "\t👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .cut_at_start = 3,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .cut_at_start = 2,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "👨d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "d",
                .cut_at_start = 1,
                .cut_at_end = 0,
            },
            {
                .substr = "d",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
            {
                .substr = "",
                .cut_at_start = 0,
                .cut_at_end = 0,
            },
        };

    static_assert(
        std::size(expected_result_substr_len) == expected_column_count + 1);
    static_assert(
        std::size(expected_result_substr_at_start)
        == expected_column_count + 1);

    bool ok = true;

    size_t column_count = ted::editor::rendered_column_count(str);

    if (column_count != expected_column_count) {
        std::cout << "  error: expecting column_count " << expected_column_count
                  << ", got " << column_count << "\n";
        ok = false;
    }

    column_count = std::min(column_count, expected_column_count);

    std::cout << "Substringing from column 0 to column " << column_count
              << "\n";
    for (size_t len = 0; len <= column_count; len++) {
        auto result = ted::editor::rendered_column_substr(str, 0, len);
        std::cout << "str.substr(0, len=" << len << ")\n";
        std::cout << "  result is '" << result.substr << "'\n";
        ok = validate(result, expected_result_substr_len[len]) && ok;
    }

    std::cout << "Substringing from column " << column_count
              << " to column 0\n";
    for (size_t start = 0; start <= column_count; start++) {
        auto result = ted::editor::rendered_column_substr(
            str,
            start,
            column_count - start);
        std::cout << "str.substr(start=" << start
                  << ", len=" << column_count - start << ")\n";
        std::cout << "  result is '" << result.substr << "'\n";
        ok = validate(result, expected_result_substr_at_start[start]) && ok;
    }

    return ok;
}

int main()
{
    ted::editor::init();

    bool ok = true;
    std::cout << "----------------------------------------------------------\n";
    std::cout << "Running test non-rendered\n";
    bool test_non_rendered_ok = run_test_non_rendered();
    if (!test_non_rendered_ok) {
        std::cout << "Test non-rendered FAILED\n";
    } else {
        std::cout << "Test non-rendered SUCCEEDED\n";
    }
    ok = test_non_rendered_ok && ok;
    std::cout << "----------------------------------------------------------\n";
    std::cout << "Running test rendered\n";
    bool test_rendered_ok = run_test_rendered();
    if (!test_rendered_ok) {
        std::cout << "Test non-rendered FAILED\n";
    } else {
        std::cout << "Test non-rendered SUCCEEDED\n";
    }
    ok = test_rendered_ok && ok;
    std::cout << "----------------------------------------------------------\n";

    ted::editor::deinit();

    return ok ? 0 : 1;
}
