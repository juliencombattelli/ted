#ifndef TED_UTF8_HPP_
#define TED_UTF8_HPP_

#include <cstdint>
#include <cstdlib>
#include <string_view>

namespace ted::utf8 {

// Functions performing global (de)initialization of the Unicode backend.
// Must be called once from the main thread.
void init();
void deinit();

// Opaque data type containing objects that can be expensive to create but
// reusable between different iterations without a complete reinitialization.
// The exact content and the resulting optimizations are dependent on the
// Unicode backend: ICU (the current used backed) allows reusing UText and
// UBreakIterator objects, saving the memory allocations to create them.
// This also allows to keep the dependency on the Unicode backend limited to
// this module.
// Be aware that a shared state will not be thread safe, and synchronization
// will be needed to use such a state.
struct IteratorState;
IteratorState* create_chars_iterator_state();
void destroy_state(IteratorState* state);

// A user-perceived character object representing a single character as
// displayed on the terminal (also called Unicode grapheme cluster).
// TODO current implementation is quite heavy and could be reduced to a single
// string_view containing the byte sequence and the column count took by it.
struct Char {
    std::string_view str;
    size_t start_byte;
    size_t next_byte;
    uint8_t column;

    [[nodiscard]]
    std::string_view to_string_view() const
    {
        return {
            str.data() + start_byte,
            str.data() + next_byte,
        };
    }
};

// TODO implement decrement
struct CharIterator {
    using value_type = Char;

    CharIterator& operator++();

    CharIterator operator++(int)
    {
        CharIterator retval = *this;
        ++(*this);
        return retval;
    }

    bool operator==(CharIterator other) const;

    bool operator!=(CharIterator other) const
    {
        return !(*this == other);
    }

    Char operator*() const;

    IteratorState* state;
    Char current_char;
};

struct Chars {
    using iterator = CharIterator;
    CharIterator begin();
    CharIterator end();
    IteratorState* state;
    std::string_view str;
};

// Create a view over the user-perceived characters (also called unicode
// grapheme clusters).
// The shared_state argument must be a valid IteratorState. It is reset when
// calling this function, so no need to reset it manually.
// TODO default shared_state to nullptr and allocate internally
Chars chars(std::string_view s, IteratorState* shared_state);

} // namespace ted::utf8

#endif // TED_UTF8_HPP_
