#ifndef TED_EDITOR_HPP_
#define TED_EDITOR_HPP_

#include <ted/key.hpp>
#include <ted/utils.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>

#define TED_VERSION_MAJOR 0
#define TED_VERSION_MINOR 1
#define TED_VERSION_PATCH 0
#define TED_VERSION                                                            \
    TED_STRINGIFY_VALUE_OF(TED_VERSION_MAJOR)                                  \
    "." TED_STRINGIFY_VALUE_OF(TED_VERSION_MINOR) "." TED_STRINGIFY_VALUE_OF(  \
        TED_VERSION_PATCH)

namespace ted::editor {

using KeyHandler = void(void* userdata);
using KeyMap = std::array<KeyHandler*, std::to_underlying(Key::Count)>;

struct State {
    std::string screen_buffer;
    size_t screen_rows = 0;
    size_t screen_cols = 0;
    size_t cursor_row = 0;
    size_t cursor_col = 0;
    char eob_char = '\0';
    KeyMap keymap;
};

extern State state;

void init(size_t rows, size_t column);

void screen_buffer_append(char c);
void screen_buffer_append(const char* s);

void cursor_up();
void cursor_down();
void cursor_left();
void cursor_right();

void set_keymap(Key::Code keycode, KeyHandler* handler);

} // namespace ted::editor

#endif // TED_EDITOR_HPP_
