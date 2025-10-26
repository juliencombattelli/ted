#include <ted/utf8.hpp>

#include <ted/os.hpp>

#include <unicode/ubrk.h>
#include <unicode/uclean.h>
#include <unicode/utext.h>
#include <unicode/utypes.h>

#include <cstddef>
#include <limits>
#include <string_view>

namespace ted::utf8 {

void init()
{
    UErrorCode status = U_ZERO_ERROR;
    u_init(&status);
    TED_ASSERT(U_SUCCESS(status));
}

void deinit()
{
    u_cleanup();
}

struct IteratorState {
    UText text = UTEXT_INITIALIZER;
    UBreakIterator* break_iterator = nullptr;
};

IteratorState* create_chars_iterator_state()
{
    // NOLINTNEXTLINE(*owning-memory*)
    auto* state = new IteratorState {};

    UErrorCode status = U_ZERO_ERROR;
    utext_openUTF8(&state->text, "", -1, &status);
    if (U_FAILURE(status)) { // NOLINT(*bool-conversion*)
        delete state; // NOLINT(*owning-memory*)
        os::exit_err("utext_openUTF8() failed");
    }

    status = U_ZERO_ERROR;
    state->break_iterator
        = ubrk_open(UBRK_CHARACTER, nullptr, u"", -1, &status);

    // Reset the break iterator to our empty UText in case additional allocation
    // occurs to handle UTF8 strings
    status = U_ZERO_ERROR;
    ubrk_setUText(state->break_iterator, &state->text, &status);

    return state;
}

void destroy_state(IteratorState* state)
{
    ubrk_close(state->break_iterator);
    utext_close(&state->text);
    delete state; // NOLINT(*owning-memory*)
}

static bool is_wide_char_at(UText* ut, size_t byte_index)
{
    TED_ASSERT(byte_index <= std::numeric_limits<int64_t>::max());
    UChar32 codepoint = utext_char32At(ut, static_cast<int64_t>(byte_index));
    int width = u_getIntPropertyValue(codepoint, UCHAR_EAST_ASIAN_WIDTH);
    return (width == U_EA_FULLWIDTH) || (width == U_EA_WIDE);
}

static uint8_t char_column(UText* ut, size_t byte_index)
{
    if (is_wide_char_at(ut, byte_index)) {
        return 2;
    }
    return 1;
}

CharIterator& CharIterator::operator++()
{
    current_char.start_byte = current_char.next_byte;
    current_char.next_byte
        = static_cast<size_t>(ubrk_next(state->break_iterator));
    current_char.column = char_column(&state->text, current_char.start_byte);
    return *this;
}

bool CharIterator::operator==(CharIterator other) const
{
    TED_ASSERT(state == other.state);
    return current_char.next_byte == other.current_char.next_byte;
}

Char CharIterator::operator*() const
{
    return current_char;
}

CharIterator Chars::begin()
{
    Char current_char {
        .str = str,
        .start_byte = static_cast<size_t>(ubrk_first(state->break_iterator)),
        .next_byte = static_cast<size_t>(ubrk_next(state->break_iterator)),
        .column = char_column(&state->text, current_char.start_byte),
    };
    return {
        .state = state,
        .current_char = current_char,
    };
}

CharIterator Chars::end()
{
    Char current_char {
        .str = str,
        .start_byte = static_cast<size_t>(UBRK_DONE),
        .next_byte = static_cast<size_t>(UBRK_DONE),
        .column = 0,
    };
    return {
        .state = state,
        .current_char = current_char,
    };
}

Chars chars(std::string_view s, IteratorState* shared_state)
{
    Chars chars { .state = shared_state, .str = s };

    TED_ASSERT(shared_state != nullptr);

    UText* ut = &shared_state->text;
    UBreakIterator* bi = shared_state->break_iterator;

    UErrorCode status = U_ZERO_ERROR;
    TED_ASSERT(s.length() <= std::numeric_limits<int64_t>::max());
    utext_openUTF8(ut, s.data(), static_cast<int64_t>(s.length()), &status);
    TED_ASSERT(U_SUCCESS(status));

    status = U_ZERO_ERROR;
    ubrk_setUText(bi, ut, &status);
    TED_ASSERT(U_SUCCESS(status));

    return chars;
}

} // namespace ted::utf8
