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
        editor::screen_buffer_append("\r\n");
    default:
        editor::screen_buffer_append(c);
    }
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
}

static void draw_eob_chars(editor::State& state)
{
    char eob_char = state.eob_char;
    for (int row = 0; row < state.screen_rows - 1; row++) {
        char eob_row[] = " \r\n";
        eob_row[0] = eob_char;
        editor::screen_buffer_append(eob_row);
    }
    editor::screen_buffer_append(eob_char);
}

static void refresh_screen()
{
    term::clear();
    draw_eob_chars(editor::state);
    term::cursor_home();
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
