#include "ted/utf8.hpp"

#include <iostream>
#include <string>
#include <string_view>

void validate(
    const ted::utf8::SubstrResult& result,
    const ted::utf8::SubstrResult& expected)
{
    if (result.substr != expected.substr) {
        std::cout << "  error: expecting substr '" << expected.substr
                  << "', got '" << result.substr << "'\n";
    }
    if (result.byte_start != expected.byte_start) {
        std::cout << "  error: expecting start " << expected.byte_start
                  << ", got " << result.byte_start << "\n";
    }
    if (result.byte_len != expected.byte_len) {
        std::cout << "  error: expecting len " << expected.byte_len << ", got "
                  << result.byte_len << "\n";
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
    ted::utf8::State* state = ted::utf8::create();

    std::string str = "a👨‍🏭b👨‍🏭c👨d";
    ted::utf8::SubstrResult expected_result_substr_len[] {
        {
            .substr = "",
            .byte_start = std::string_view::npos,
            .byte_len = std::string_view::npos,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "a",
            .byte_start = 0,
            .byte_len = 1,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "a",
            .byte_start = 0,
            .byte_len = 1,
            .cut_at_start = false,
            .cut_at_end = true,
        },
        {
            .substr = "a👨‍🏭",
            .byte_start = 0,
            .byte_len = 12,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "a👨‍🏭b",
            .byte_start = 0,
            .byte_len = 13,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "a👨‍🏭b",
            .byte_start = 0,
            .byte_len = 13,
            .cut_at_start = false,
            .cut_at_end = true,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭",
            .byte_start = 0,
            .byte_len = 24,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭c",
            .byte_start = 0,
            .byte_len = 25,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭c",
            .byte_start = 0,
            .byte_len = 25,
            .cut_at_start = false,
            .cut_at_end = true,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭c👨",
            .byte_start = 0,
            .byte_len = 29,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "a👨‍🏭b👨‍🏭c👨d",
            .byte_start = 0,
            .byte_len = 30,
            .cut_at_start = false,
            .cut_at_end = false,
        },
    };
    ted::utf8::SubstrResult expected_result_substr_at_start[] {
        {
            .substr = "a👨‍🏭b👨‍🏭c👨d",
            .byte_start = 0,
            .byte_len = 30,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "👨‍🏭b👨‍🏭c👨d",
            .byte_start = 1,
            .byte_len = 29,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "b👨‍🏭c👨d",
            .byte_start = 12,
            .byte_len = 18,
            .cut_at_start = true,
            .cut_at_end = false,
        },
        {
            .substr = "b👨‍🏭c👨d",
            .byte_start = 12,
            .byte_len = 18,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "👨‍🏭c👨d",
            .byte_start = 13,
            .byte_len = 17,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "c👨d",
            .byte_start = 24,
            .byte_len = 6,
            .cut_at_start = true,
            .cut_at_end = false,
        },
        {
            .substr = "c👨d",
            .byte_start = 24,
            .byte_len = 6,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "👨d",
            .byte_start = 25,
            .byte_len = 5,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "d",
            .byte_start = 29,
            .byte_len = 1,
            .cut_at_start = true,
            .cut_at_end = false,
        },
        {
            .substr = "d",
            .byte_start = 29,
            .byte_len = 1,
            .cut_at_start = false,
            .cut_at_end = false,
        },
        {
            .substr = "",
            .byte_start = std::string_view::npos,
            .byte_len = std::string_view::npos,
            .cut_at_start = false,
            .cut_at_end = false,
        },
    };

    auto str_len = ted::utf8::strlen(state, str);
    std::cout << "len=" << str_len << "\n";

    std::cout << "\n";

    for (size_t len = 0; len <= str_len; len++) {
        auto result = ted::utf8::substr(state, str, 0, len);
        std::cout << "str.substr(0, len=" << len << ")\n";
        std::cout << "  result is '" << result.substr << "'\n";
        validate(result, expected_result_substr_len[len]);
    }

    std::cout << "\n";

    for (size_t start = 0; start <= str_len; start++) {
        auto result = ted::utf8::substr(state, str, start, str_len - start);
        std::cout << "str.substr(start=" << start << ", len=" << str_len - start
                  << ")\n";
        std::cout << "  result is '" << result.substr << "'\n";
        validate(result, expected_result_substr_at_start[start]);
    }

    ted::utf8::destroy(state);
    ted::utf8::deinit();
}
