#include <ted/editor.hpp>
#include <ted/term.hpp>

#include <array>
#include <bit>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <utility>

namespace ted::term {

static void send_code(const char* code)
{
    editor::screen_buffer_append(code);
}

void cursor_move(size_t row, size_t col)
{
    // Number of char to represent any size_t + 1 of how digits10 is defined
    // Example: a 8-bit integer type can represent any two-digit number exactly,
    // but 3-digit decimal numbers 256..999 cannot be represented
    static constexpr size_t size_t_digit_count
        = std::numeric_limits<size_t>::digits10 + 1;

    // Minimal char count for a valid code "\e[0;0H"
    static constexpr size_t min_code_len = 6;

    // Buffer size rounded up to the greater power of 2
    static constexpr size_t code_buf_size
        = std::bit_ceil(min_code_len + (size_t_digit_count * 2));

    char code[code_buf_size] {};
    int n = snprintf(code, code_buf_size, "\e[%zu;%zuH", row + 1, col + 1);
    assert(min_code_len <= n && n <= code_buf_size);

    send_code(code);
}

void cursor_home()
{
    send_code("\e[H");
}

void cursor_show()
{
    send_code("\e[?25h");
}

void cursor_hide()
{
    send_code("\e[?25l");
}

void erase_line(EraseLineMode mode)
{
    static constexpr std::array codes {
        "\e[0K",
        "\e[1K",
        "\e[2K",
    };
    assert(std::to_underlying(mode) < codes.size());
    const char* code = codes[std::to_underlying(mode)];
    send_code(code);
}

void erase_line()
{
    send_code("\e[K");
}

void clear(ClearMode mode)
{
    static constexpr std::array codes {
        "\e[0J",
        "\e[1J",
        "\e[2J",
    };
    assert(std::to_underlying(mode) < codes.size());
    const char* code = codes[std::to_underlying(mode)];
    send_code(code);
}

void clear()
{
    send_code("\e[J");
}

} // namespace ted::term
