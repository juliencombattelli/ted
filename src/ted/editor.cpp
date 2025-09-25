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

void screen_buffer_append(char c)
{
    state.screen_buffer.push_back(c);
}
void screen_buffer_append(const char* s)
{
    state.screen_buffer.append(s);
}

void cursor_up()
{
    if (state.cursor_row > 0) {
        state.cursor_row--;
    }
}
void cursor_down()
{
    if (state.cursor_row < state.screen_size.rows - 1) {
        state.cursor_row++;
    }
}
void cursor_left()
{
    if (state.cursor_col > 0) {
        state.cursor_col--;
    }
}
void cursor_right()
{
    if (state.cursor_col < state.screen_size.cols - 1) {
        state.cursor_col++;
    }
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
