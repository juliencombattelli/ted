#include "ted/editor.hpp"
#include "ted/utf8.hpp"

#include <iostream>
#include <string>
#include <string_view>

static void validate(
    const ted::editor::SubstrResult& result,
    const ted::editor::SubstrResult& expected)
{
    if (result.substr != expected.substr) {
        std::cout << "  error: expecting substr '" << expected.substr
                  << "', got '" << result.substr << "'\n";
    }
    if (result.cut_at_start != expected.cut_at_start) {
        std::cout << "  error: expecting cut_at_start " << expected.cut_at_start
                  << ", got " << result.cut_at_start << "\n";
    }
    if (result.cut_at_end != expected.cut_at_end) {
        std::cout << "  error: expecting cut_at_end " << expected.cut_at_end
                  << ", got " << result.cut_at_end << "\n";
    }
}

int main()
{
    ted::utf8::init();
    ted::editor::state.utf8_chars_iterator_state
        = ted::utf8::create_chars_iterator_state();

    std::string str = "a👨‍🏭b👨‍🏭c👨d";
    ted::editor::SubstrResult expected_result_substr_len[] {
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
            .substr = "a👨‍🏭b👨‍🏭c",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭c",
            .cut_at_start = 0,
            .cut_at_end = 1,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭c👨",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭c👨d",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
    };
    ted::editor::SubstrResult expected_result_substr_at_start[] {
        {
            .substr = "a👨‍🏭b👨‍🏭c👨d",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "👨‍🏭b👨‍🏭c👨d",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "b👨‍🏭c👨d",
            .cut_at_start = 1,
            .cut_at_end = 0,
        },
        {
            .substr = "b👨‍🏭c👨d",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "👨‍🏭c👨d",
            .cut_at_start = 0,
            .cut_at_end = 0,
        },
        {
            .substr = "c👨d",
            .cut_at_start = 1,
            .cut_at_end = 0,
        },
        {
            .substr = "c👨d",
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

    auto str_len = ted::editor::column_count(str);
    std::cout << "len=" << str_len << "\n";

    std::cout << "\n";

    for (size_t len = 0; len <= str_len; len++) {
        auto result = ted::editor::column_substr(str, 0, len);
        std::cout << "str.substr(0, len=" << len << ")\n";
        std::cout << "  result is '" << result.substr << "'\n";
        validate(result, expected_result_substr_len[len]);
    }

    std::cout << "\n";

    for (size_t start = 0; start <= str_len; start++) {
        auto result = ted::editor::column_substr(str, start, str_len - start);
        std::cout << "str.substr(start=" << start << ", len=" << str_len - start
                  << ")\n";
        std::cout << "  result is '" << result.substr << "'\n";
        validate(result, expected_result_substr_at_start[start]);
    }

    ted::utf8::destroy_state(ted::editor::state.utf8_chars_iterator_state);
    ted::utf8::deinit();
}
