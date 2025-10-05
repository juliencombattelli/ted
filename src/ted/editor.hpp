#ifndef TED_EDITOR_HPP_
#define TED_EDITOR_HPP_

#include <ted/key.hpp>
#include <ted/utf8.hpp>
#include <ted/utils.hpp>

#include <array>
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

struct Line {
    std::string bytes;

    [[nodiscard]]
    size_t length() const;

    [[nodiscard]]
    utf8::SubstrResult substr(size_t pos, size_t n) const;

    [[nodiscard]]
    std::string_view at(size_t col) const;
};

struct File {
    std::vector<Line> lines;
};

struct ScreenSize {
    size_t rows {};
    size_t cols {};
};

struct Coord {
    size_t row {};
    size_t col {};
};

struct Config {
    KeyMap keymap;
    char eob_char;
    std::string tab_str;
    uint8_t tab_width;
    bool debug_enabled;
};

struct State {
    std::vector<File> opened_files;
    File* viewed_file;
    std::string screen_buffer;
    ScreenSize screen_size;
    Coord cursor_coord; // Cursor coordinate in file, not on screen
    size_t cursor_col_memorized {};
    Coord viewport_offset;
    utf8::State* utf8;
    std::string full_tab_string;
    Config config;
};

// NOLINTNEXTLINE(*global*)
extern State state;

void dump_state_open(const char* filename);
void dump_state_close();
void dump_state();

void init();
void deinit();

void screen_buffer_append_char(char c);
void screen_buffer_append(const char* s);
void screen_buffer_append_n(const char* s, size_t n);
void screen_buffer_append_substr(const utf8::SubstrResult& substr);

void scroll();

void cursor_up();
void cursor_down();
void cursor_left();
void cursor_right();
void cursor_start_of_line();
void cursor_end_of_line();

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

void set_tab_width(uint8_t width);
uint8_t get_tab_width();

void set_tab_string(std::string_view s);
std::string_view get_tab_string();

void set_keymap(Key::Code keycode, KeyHandler* handler);
KeyHandler* get_keymap(Key::Code keycode);

void open_new_file();
void open_file(const char* path);

} // namespace ted::editor

#endif // TED_EDITOR_HPP_
