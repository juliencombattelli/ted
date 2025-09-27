#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

#include <fstream>

namespace ted::editor {

State state;

void init()
{
    // Load default configuration
    state.eob_char = '~';
    // Keymap is not initialized here as the default mapping could change
    // between a TUI or GUI mode
}

void screen_buffer_append_char(char c)
{
    state.screen_buffer.push_back(c);
}
void screen_buffer_append(const char* s)
{
    state.screen_buffer.append(s);
}
void screen_buffer_append_n(const char* s, size_t n)
{
    state.screen_buffer.append(s, n);
}

void scroll()
{
    auto& viewport_row = editor::state.viewport_offset.row;
    auto& viewport_col = editor::state.viewport_offset.col;

    if (editor::get_cursor_row() < viewport_row) {
        viewport_row = editor::get_cursor_row();
    }
    if (editor::get_cursor_row() >= viewport_row + editor::get_screen_rows()) {
        viewport_row = editor::get_cursor_row() - editor::get_screen_rows() + 1;
    }
    if (editor::get_cursor_col() < viewport_col) {
        viewport_col = editor::get_cursor_col();
    }
    if (editor::get_cursor_col() >= viewport_col + editor::get_screen_cols()) {
        viewport_col = editor::get_cursor_col() - editor::get_screen_cols() + 1;
    }
}

void cursor_up()
{
    if (state.cursor_coord.row > 0) {
        state.cursor_coord.row--;
    }
}
void cursor_down()
{
    if (state.cursor_coord.row < state.viewed_file->lines.size() - 1) {
        state.cursor_coord.row++;
    }
}
void cursor_left()
{
    if (state.cursor_coord.col > 0) {
        state.cursor_coord.col--;
    }
}
void cursor_right()
{
    // if (state.cursor_coord.col < state.screen_size.cols - 1) {
    state.cursor_coord.col++;
    // }
}

void set_cursor_row(size_t row)
{
    state.cursor_coord.row = std::min(row, state.screen_size.rows - 1);
}
void set_cursor_row_top()
{
    state.cursor_coord.row = 0;
}
void set_cursor_row_bot()
{
    state.cursor_coord.row = state.screen_size.rows - 1;
}
size_t get_cursor_row()
{
    return state.cursor_coord.row;
}

void set_cursor_col(size_t col)
{
    state.cursor_coord.col = std::min(col, state.screen_size.cols - 1);
}
void set_cursor_col_left()
{
    state.cursor_coord.col = 0;
}
void set_cursor_col_right()
{
    state.cursor_coord.col = state.screen_size.cols - 1;
}
size_t get_cursor_col()
{
    return state.cursor_coord.col;
}

void set_screen_size(ScreenSize screen_size)
{
    state.screen_size = screen_size;
    state.screen_buffer.reserve(screen_size.rows * screen_size.cols);
}
ScreenSize get_screen_size()
{
    return state.screen_size;
}
void set_screen_rows(size_t rows)
{
    set_screen_size(ScreenSize { rows, get_screen_cols() });
}
size_t get_screen_rows()
{
    return state.screen_size.rows;
}
void set_screen_cols(size_t cols)
{
    set_screen_size(ScreenSize { get_screen_rows(), cols });
}
size_t get_screen_cols()
{
    return state.screen_size.cols;
}

void set_keymap(Key::Code keycode, KeyHandler* handler)
{
    state.keymap[keycode] = handler;
}

KeyHandler* get_keymap(Key::Code keycode)
{
    return state.keymap[keycode];
}

void open_new_file()
{
    state.viewed_file = &state.opened_files.emplace_back();
    // TODO handle newline type depending on settings
    state.viewed_file->lines.emplace_back("\r\n");
}
void open_file(const char* path)
{
    state.viewed_file = &state.opened_files.emplace_back();

    std::fstream file(path);
    if (!file.is_open()) {
        os::exit_err_format("Cannot open file {}", path);
    }

    for (std::string line; std::getline(file, line);) {
        state.viewed_file->lines.emplace_back(line);
    }
}

} // namespace ted::editor
