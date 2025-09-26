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
    std::vector<std::string> lines;
};

struct ScreenSize {
    size_t rows {};
    size_t cols {};
};

struct Coord {
    size_t row {};
    size_t col {};
};

struct State {
    std::vector<File> opened_files;
    File* viewed_file;
    std::string screen_buffer;
    ScreenSize screen_size;
    Coord cursor_coord;
    Coord viewport_offset;
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

void set_cursor_row(size_t row);
void set_cursor_row_top();
void set_cursor_row_bot();
size_t get_cursor_row();

void set_cursor_col(size_t col);
void set_cursor_col_left();
void set_cursor_col_right();
size_t get_cursor_col();

void set_screen_rows(size_t rows);
size_t get_screen_rows();
void set_screen_cols(size_t cols);
size_t get_screen_cols();

void set_keymap(Key::Code keycode, KeyHandler* handler);
KeyHandler* get_keymap(Key::Code keycode);

void open_new_file();
void open_file(const char* path);

} // namespace ted::editor

#endif // TED_EDITOR_HPP_
