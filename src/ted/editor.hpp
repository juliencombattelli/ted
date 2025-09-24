#ifndef TED_EDITOR_HPP_
#define TED_EDITOR_HPP_

#include <ted/key.hpp>
#include <ted/utils.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

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

struct File {
    File()
    {
        // TODO handle newline type depending on settings
        lines.emplace_back("\n");
    }
    std::vector<std::string> lines;
};

struct ScreenSize {
    size_t rows {};
    size_t cols {};
};

struct State {
    std::vector<File> opened_files;
    File* viewed_file;
    std::string screen_buffer;
    ScreenSize screen_size;
    size_t cursor_row = 0;
    size_t cursor_col = 0;
    char eob_char;
    KeyMap keymap;
};

extern State state;

void init();

void screen_buffer_append(char c);
void screen_buffer_append(const char* s);

void cursor_up();
void cursor_down();
void cursor_left();
void cursor_right();

void set_screen_rows(size_t rows);
size_t get_screen_rows();
void set_screen_cols(size_t cols);
size_t get_screen_cols();

void set_keymap(Key::Code keycode, KeyHandler* handler);
KeyHandler* get_keymap(Key::Code keycode);

void open_new_file();

} // namespace ted::editor

#endif // TED_EDITOR_HPP_
