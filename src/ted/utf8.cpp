
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
    UBreakIterator* it;
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
    state->it = ubrk_open(UBRK_CHARACTER, nullptr, u"", -1, &status);

    // Reset the break iterator to our empty UText in case additional allocation
    // occurs to handle UTF8 strings
    status = U_ZERO_ERROR;
    ubrk_setUText(state->it, state->utext, &status);

    return state;
}

void destroy(State* state)
{
    ubrk_close(state->it);
    utext_close(state->utext);
    delete state;
}

size_t strlen(State* state, std::string_view s)
{
    assert(state != nullptr);

    UText* ut = state->utext;
    UBreakIterator* it = state->it;

    UErrorCode status = U_ZERO_ERROR;
    assert(s.length() <= std::numeric_limits<int64_t>::max());
    utext_openUTF8(ut, s.data(), static_cast<int64_t>(s.length()), &status);
    assert(U_SUCCESS(status));

    status = U_ZERO_ERROR;
    ubrk_setUText(it, ut, &status);
    assert(U_SUCCESS(status));

    size_t char_index = 0;
    size_t start = 0;
    size_t len = std::string_view::npos;
    int32_t byte_index = ubrk_first(it);
    while (byte_index != UBRK_DONE) {
        char_index++;
        byte_index = ubrk_next(it);
    }
    return char_index;
}

std::string_view substr(State* state, std::string_view s, size_t pos, size_t n)
{
    assert(state != nullptr);

    UText* ut = state->utext;
    UBreakIterator* it = state->it;

    UErrorCode status = U_ZERO_ERROR;
    assert(s.length() <= std::numeric_limits<int64_t>::max());
    utext_openUTF8(ut, s.data(), static_cast<int64_t>(s.length()), &status);
    assert(U_SUCCESS(status));

    status = U_ZERO_ERROR;
    ubrk_setUText(it, ut, &status);
    assert(U_SUCCESS(status));

    size_t char_index = 0;
    size_t start = 0;
    size_t len = std::string_view::npos;
    int32_t byte_index = ubrk_first(it);
    while (byte_index != UBRK_DONE) {
        if (char_index == pos) {
            start = byte_index;
        }
        if (char_index == pos + n) {
            len = byte_index - start;
            break;
        }
        char_index++;
        byte_index = ubrk_next(it);
    }
    return s.substr(start, len);
}

} // namespace ted::utf8
