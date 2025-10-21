#ifndef TED_EDITOR_HPP_
#define TED_EDITOR_HPP_

#include <ted/config.hpp>
#include <ted/key.hpp>
#include <ted/utf8.hpp>
#include <ted/utils.hpp>

#include <array>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

namespace ted::editor {

using KeyHandler = void(void* userdata);
using KeyMap = std::array<KeyHandler*, std::to_underlying(Key::Count)>;

// Result returned by column substring operations
//
// Example:
// col_pos = 7
// col_n = 13
// col_pos + col_n ──────────────────────────┐
// col_pos ──────────────────────┐           │
//                               │           │
// input string =        "start> █ >   >   > █ >   >   end"
//                             ││││        │ │
// offset_from_start_char = 2 ─┴┘││        │ │
// cut_at_start = 2 ─────────────┴┘        │ │
// cut_at_end = 3 ─────────────────────────┴─┘
//
// offset_to_last_char is not stored as it is not necessary for now
//
// The use-case above is used as a reference test in test/utf8_test.cpp
struct SubstrResult {
    std::string_view substr;
    // column offset from beginning of char if start is cut
    size_t offset_from_start_char;
    // column count cut at start or end
    size_t cut_at_start;
    size_t cut_at_end;
};

uint8_t rendered_char_column_width(const ted::utf8::Char& ch);
size_t column_count(std::string_view str);
size_t rendered_column_count(std::string_view str);
SubstrResult column_substr(std::string_view str, size_t col_pos, size_t col_n);
SubstrResult rendered_column_substr(
    std::string_view str,
    size_t col_pos,
    size_t col_n);

using Line = std::string;

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
    // Cursor coordinate in file, not on screen
    Coord cursor_coord;
    // Cursor column in file as rendered on the screen (eg. with tabs expanded),
    // what the user really perceives as cursor coord
    size_t cursor_col_rendered {};
    size_t cursor_col_memorized {};
    Coord viewport_offset;
    utf8::IteratorState* utf8_chars_iterator_state;
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
void screen_buffer_append_substr(const SubstrResult& substr);

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
