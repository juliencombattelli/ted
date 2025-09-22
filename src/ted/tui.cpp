#include <climits>
#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

#include <cassert>
#include <cctype>
#include <cstdio>
#include <utility>

namespace ted::tui {

[[nodiscard]]
static char read_key()
{
    int c = std::fgetc(stdin);
    if (c == EOF) {
        if (std::ferror(stdin)) {
            os::exit_err("fread() failed");
        }
        if (std::feof(stdin)) {
            os::exit_err("fread() end-of-file", 0);
        }
        std::unreachable();
    }
    assert(c <= UCHAR_MAX);
    return (char)c;
}

static void process_key(char c)
{
    fprintf(stderr, "key pressed: %d (%c)\n", c, c);

    auto key_handler = editor::state.keymap[c];
    if (key_handler != nullptr) {
        // TODO handle userdata
        key_handler(nullptr);
    } else {
        editor::screen_buffer_append(c);
    }
}

static void load_default_tui_keymap()
{
    // TODO replace with arrow keys
    editor::state.keymap['z'] = [](void*) { editor::cursor_up(); };
    editor::state.keymap['s'] = [](void*) { editor::cursor_down(); };
    editor::state.keymap['d'] = [](void*) { editor::cursor_right(); };
    editor::state.keymap['q'] = [](void*) { editor::cursor_left(); };

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
        char eob_row[] = " \r\n";
        eob_row[0] = eob_char;
        editor::screen_buffer_append(eob_row);
        term::erase_line();
    }
    editor::screen_buffer_append(eob_char);
}

static void refresh_screen()
{
    term::cursor_hide();
    // term::clear();
    draw_eob_chars(editor::state);
    term::cursor_move(editor::state.cursor_row, editor::state.cursor_col);
    term::cursor_show();

    std::string& screen_buffer = editor::state.screen_buffer;
    // TODO handle write error?
    (void)std::fwrite(screen_buffer.data(), 1, screen_buffer.length(), stdout);
    screen_buffer.clear();
}

void start()
{
    init();
    while (true) {
        refresh_screen();
        char c = read_key();
        process_key(c);
    }
}

} // namespace ted::tui
