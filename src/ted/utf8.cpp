#include <ted/utf8.hpp>

#include <ted/os.hpp>

#include <unicode/ubrk.h>
#include <unicode/uclean.h>
#include <unicode/utext.h>
#include <unicode/utypes.h>

#include <cassert>
#include <limits>
#include <type_traits>

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

enum CharsIteration {
    Continue,
    Break,
};

struct CharsIteratorContext {
    std::string_view input;
    size_t col_count;
    int32_t byte_index;
    bool wide_codepoint;
};

static CharsIteration chars_it_do_nothing(const CharsIteratorContext& /*ctx*/)
{
    return CharsIteration::Continue;
};

// Iterate over each unicode characters (grapheme clusters), calling fn for each
// one, and return the number of characters iterated
template<typename Fn>
    requires std::is_invocable_r_v<CharsIteration, Fn, CharsIteratorContext>
static size_t for_each_chars(State* state, std::string_view s, Fn&& fn)
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

    CharsIteratorContext it_ctx {
        .input = s,
        .col_count = 0,
        .byte_index = ubrk_first(it),
        .wide_codepoint = false,
    };

    while (it_ctx.byte_index != UBRK_DONE) {
        it_ctx.wide_codepoint = is_wide_codepoint_at(ut, it_ctx.byte_index);

        CharsIteration iteration = fn(it_ctx);
        if (iteration == CharsIteration::Break) {
            break;
        }

        if (it_ctx.wide_codepoint) {
            it_ctx.col_count++;
        }
        it_ctx.col_count++;
        it_ctx.byte_index = ubrk_next(it);
    }
    return it_ctx.col_count - 1;
}

size_t strlen(State* state, std::string_view s)
{
    return for_each_chars(state, s, chars_it_do_nothing);
}

SubstrResult substr(
    State* state,
    std::string_view s,
    size_t col_pos,
    size_t col_n)
{
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

    for_each_chars(state, s, [&](const CharsIteratorContext& it_ctx) {
        if (result.byte_start == npos && it_ctx.col_count >= col_pos) {
            result.byte_start = it_ctx.byte_index;
            if (it_ctx.col_count > col_pos) {
                result.cut_at_start = true;
            }
        }
        size_t col_end = col_pos + col_n - (it_ctx.wide_codepoint ? 1 : 0);
        if (it_ctx.col_count >= col_end) {
            result.byte_len = it_ctx.byte_index - result.byte_start;
            if (it_ctx.wide_codepoint && it_ctx.col_count == col_end) {
                result.cut_at_end = true;
            }
            return CharsIteration::Break;
        }
        return CharsIteration::Continue;
    });

    result.substr = s.substr(result.byte_start, result.byte_len);
    return result;
}

} // namespace ted::utf8
