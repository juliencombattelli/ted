#include <ted/editor.hpp>
#include <ted/term.hpp>

#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <utility>

namespace ted::term {

static void send_code(const char* code)
{
    editor::screen_buffer_append(code);
}

void cursor_home()
{
    send_code("\e[H");
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
    cursor_home();
}

} // namespace ted::term
