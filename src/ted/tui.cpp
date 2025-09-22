#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

#include <cctype>
#include <cstdio>
#include <utility>

namespace ted::tui {

[[nodiscard]]
static int read_key()
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
    return c;
}

static void process_key(int c)
{
    // if (std::iscntrl(c)) {
    //     std::printf("%d\r\n", c);
    // } else {
    //     std::printf("%d ('%c')\r\n", c, c);
    // }

    switch (c) {
    case term::key_ctrl('q'):
        ted::os::exit_ok();
        break;
    case term::key_ctrl('c'):
        term::clear();
        break;
    case '\r':
        std::putchar('\r');
        std::putchar('\n');
    default:
        std::putchar(c);
    }
}

static void init(editor::State& state)
{
    term::init();
    if (!term::get_size(state.screen_rows, state.screen_cols)) {
        // Fallback to escape sequence computing
        os::exit_err("ted::term::get_size() failed");
    }
}

static void draw_eob_chars(editor::State& state)
{
    for (int row = 0; row < state.screen_rows; row++) {
        char eob_row[] = " \r\n";
        eob_row[0] = state.eob_char;
        (void)std::fputs(eob_row, stdout);
    }
}

static void refresh_screen(editor::State& state)
{
    term::clear();
    draw_eob_chars(state);
    term::cursor_home();
}

void start(editor::State& state)
{
    tui::init(state);
    int c = 0;
    while (true) {
        refresh_screen(state);
        int c = read_key();
        process_key(c);
    }
}

} // namespace ted::tui