#include <ted/utf8.hpp>

#include <ted/os.hpp>

#include <unicode/ubrk.h>
#include <unicode/uclean.h>
#include <unicode/utext.h>
#include <unicode/utypes.h>

#include <cassert>
#include <limits>

namespace ted::utf8 {

void init()
{
    UErrorCode status = U_ZERO_ERROR;
    u_init(&status);
    assert(U_SUCCESS(status));
}

void deinit()
{
    u_cleanup();
}

struct State {
    UText* utext;
    UBreakIterator* char_it;
};

State* create()
{
    State* state = new State {};

    UErrorCode status = U_ZERO_ERROR;
    state->utext = utext_openUTF8(nullptr, "", -1, &status);
    if (U_FAILURE(status)) {
        delete state;
        os::exit_err("utext_openUTF8() failed");
    }

    status = U_ZERO_ERROR;
    state->char_it = ubrk_open(UBRK_CHARACTER, nullptr, u"", -1, &status);

    // Reset the break iterator to our empty UText in case additional allocation
    // occurs to handle UTF8 strings
    status = U_ZERO_ERROR;
    ubrk_setUText(state->char_it, state->utext, &status);

    return state;
}

void destroy(State* state)
{
    ubrk_close(state->char_it);
    utext_close(state->utext);
    delete state;
}

static bool is_wide_codepoint_at(UText* ut, int64_t byte_index)
{
    UChar32 codepoint = utext_char32At(ut, byte_index);
    int width = u_getIntPropertyValue(codepoint, UCHAR_EAST_ASIAN_WIDTH);
    return (width == U_EA_FULLWIDTH) || (width == U_EA_WIDE);
}

size_t strlen(State* state, std::string_view s)
{
    assert(state != nullptr);

    UText* ut = state->utext;
    UBreakIterator* it = state->char_it;

    UErrorCode status = U_ZERO_ERROR;
    assert(s.length() <= std::numeric_limits<int64_t>::max());
    utext_openUTF8(ut, s.data(), static_cast<int64_t>(s.length()), &status);
    assert(U_SUCCESS(status));

    status = U_ZERO_ERROR;
    ubrk_setUText(it, ut, &status);
    assert(U_SUCCESS(status));

    size_t char_count = 0;
    int32_t byte_index = ubrk_first(it);
    while (byte_index != UBRK_DONE) {
        if (is_wide_codepoint_at(ut, byte_index)) {
            char_count++;
        }
        char_count++;
        byte_index = ubrk_next(it);
    }
    return char_count - 1;
}

SubstrResult substr(
    State* state,
    std::string_view s,
    size_t col_pos,
    size_t col_n)
{
    assert(state != nullptr);

    UText* ut = state->utext;
    UBreakIterator* it = state->char_it;

    UErrorCode status = U_ZERO_ERROR;
    assert(s.length() <= std::numeric_limits<int64_t>::max());
    utext_openUTF8(ut, s.data(), static_cast<int64_t>(s.length()), &status);
    assert(U_SUCCESS(status));

    status = U_ZERO_ERROR;
    ubrk_setUText(it, ut, &status);
    assert(U_SUCCESS(status));

    static constexpr auto npos = std::string_view::npos;

    SubstrResult result {
        .substr {},
        .byte_start = npos,
        .byte_len = npos,
        .cut_at_start = false,
        .cut_at_end = false,
    };

    if (col_n == 0) {
        return result;
    }

    size_t col_count = 0;
    int32_t byte_index = ubrk_first(it);
    while (byte_index != UBRK_DONE) {
        bool wide_codepoint = is_wide_codepoint_at(ut, byte_index);
        if (result.byte_start == npos && col_count >= col_pos) {
            result.byte_start = byte_index;
            if (col_count > col_pos) {
                result.cut_at_start = true;
            }
        }
        auto col_end = col_pos + col_n - (wide_codepoint ? 1 : 0);
        if (col_count >= col_end) {
            result.byte_len = byte_index - result.byte_start;
            if (wide_codepoint && col_count == col_end) {
                result.cut_at_end = true;
            }
            break;
        }
        if (wide_codepoint) {
            col_count++;
        }
        col_count++;
        byte_index = ubrk_next(it);
    }
    result.substr = s.substr(result.byte_start, result.byte_len);
    return result;
}

} // namespace ted::utf8
