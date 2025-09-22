#include <ted/editor.hpp>
#include <ted/key.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

#include <cassert>
#include <cctype>
#include <climits>
#include <cstdio>

namespace ted::tui {

static void process_key(Key::Code keycode)
{
    auto key_handler = editor::state.keymap[keycode];
    if (key_handler != nullptr) {
        // TODO handle userdata
        key_handler(nullptr);
    } else {
        // TODO if visible char only
        editor::screen_buffer_append(keycode);
    }
}

static void load_default_tui_keymap()
{
    editor::state.keymap[Key::Code::Up] = [](void*) { editor::cursor_up(); };
    editor::state.keymap[Key::Code::Down]
        = [](void*) { editor::cursor_down(); };
    editor::state.keymap[Key::Code::Right]
        = [](void*) { editor::cursor_right(); };
    editor::state.keymap[Key::Code::Left]
        = [](void*) { editor::cursor_left(); };

    editor::state.keymap['\r']
        = [](void*) { editor::screen_buffer_append("\r\n"); };

    editor::state.keymap[term::key_ctrl('q')] = [](void*) { os::exit_ok(); };
}

static void init()
{
    size_t rows = 0, cols = 0;
    term::init();
    if (!term::get_size(rows, cols)) {
        // Fallback to escape sequence computing
        os::exit_err("ted::term::get_size() failed");
    }
    editor::init(rows, cols);
    load_default_tui_keymap();
}

static void draw_eob_chars(editor::State& state)
{
    char eob_char = state.eob_char;
    for (int row = 0; row < state.screen_rows - 1; row++) {
        editor::screen_buffer_append(eob_char);
        term::erase_line();
        editor::screen_buffer_append("\r\n");
    }
    editor::screen_buffer_append(eob_char);
    term::erase_line();
}

static void refresh_screen()
{
    term::cursor_hide();
    term::cursor_home();

    draw_eob_chars(editor::state);

    term::cursor_move(editor::state.cursor_row, editor::state.cursor_col);
    term::cursor_show();

    term::write_screen_buffer();
}

void start()
{
    init();
    while (true) {
        refresh_screen();
        Key::Code keycode = term::read_key();
        process_key(keycode);
    }
}

} // namespace ted::tui
