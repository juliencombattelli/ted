#include <ted/editor.hpp>
#include <ted/key.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <climits>
#include <cstdio>
#include <format>

namespace ted::tui {

[[nodiscard]]
static Key::Code read_escape_sequence()
{
    uint8_t seq[3];

    if (!term::read_key(seq[0])) {
        return Key::Code { '\e' };
    }
    if (!term::read_key(seq[1])) {
        return Key::Code { '\e' };
    }
    if (seq[0] == '[') {
        if (seq[1] >= '0' && seq[1] <= '9') {
            if (!term::read_key(seq[2])) {
                return Key::Code { '\e' };
            }
            if (seq[2] == '~') {
                switch (seq[1]) {
                case '1':
                    return Key::Code::Home;
                case '4':
                    return Key::Code::End;
                case '5':
                    return Key::Code::PageUp;
                case '6':
                    return Key::Code::PageDown;
                case '7':
                    return Key::Code::Home;
                case '8':
                    return Key::Code::End;
                default:
                    break;
                }
            }
        } else {
            switch (seq[1]) {
            case 'A':
                return Key::Code::Up;
            case 'B':
                return Key::Code::Down;
            case 'C':
                return Key::Code::Right;
            case 'D':
                return Key::Code::Left;
            case 'H':
                return Key::Code::Home;
            case 'F':
                return Key::Code::End;
            default:
                break;
            }
        }
    } else if (seq[0] == 'O') {
        switch (seq[1]) {
        case 'H':
            return Key::Code::Home;
        case 'F':
            return Key::Code::End;
        default:
            break;
        }
    }
    return Key::Code { '\e' };
}

[[nodiscard]]
static Key::Code read_key()
{
    uint8_t byte = 0;
    while (!term::read_key(byte)) { }
    if (byte == '\e') {
        return read_escape_sequence();
    }
    return Key::Code { byte };
}

static void process_key(Key::Code keycode)
{
    auto key_handler = editor::get_keymap(keycode);
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
    editor::set_keymap(Key::Code::Up, [](void*) { editor::cursor_up(); });
    editor::set_keymap(Key::Code::Down, [](void*) { editor::cursor_down(); });
    editor::set_keymap(Key::Code::Right, [](void*) { editor::cursor_right(); });
    editor::set_keymap(Key::Code::Left, [](void*) { editor::cursor_left(); });

    editor::set_keymap(Key::Code::PageUp, [](void*) {
        size_t times = editor::get_screen_rows();
        while (times--) {
            editor::cursor_up();
        }
    });
    editor::set_keymap(Key::Code::PageDown, [](void*) {
        size_t times = editor::get_screen_rows();
        while (times--) {
            editor::cursor_down();
        }
    });

    editor::set_keymap(Key::Code::Home, [](void*) {
        editor::state.cursor_col = 0;
    });
    editor::set_keymap(Key::Code::End, [](void*) {
        editor::state.cursor_col = editor::get_screen_cols() - 1;
    });

    editor::set_keymap(Key::Code { '\r' }, [](void*) {
        editor::screen_buffer_append("\r\n");
    });

    editor::set_keymap(Key::Code::CtrlQ, [](void*) { os::exit_ok(); });
}

static void handle_resize()
{
    // Handle window resizing
    size_t rows = 0;
    size_t cols = 0;
    if (!term::get_size(rows, cols)) {
        // TODO fallback to escape sequence computing
        os::exit_err("ted::term::get_size() failed");
    }
    size_t screen_size_rows = editor::get_screen_rows();
    size_t screen_size_cols = editor::get_screen_cols();
    if (rows != screen_size_rows || cols != screen_size_cols) {
        editor::set_screen_rows(rows);
        editor::set_screen_cols(cols);
        // TODO consider clearing the screen in case of artifacts on resize
        // term::clear();
    }
}

void init()
{
    size_t rows = 0;
    size_t cols = 0;
    term::init();
    if (!term::get_size(rows, cols)) {
        // TODO fallback to escape sequence computing
        os::exit_err("ted::term::get_size() failed");
    }
    editor::set_screen_rows(rows);
    editor::set_screen_cols(cols);
    load_default_tui_keymap();
}

static constexpr std::string_view welcome_message[] {
    "Ted v" TED_VERSION,
    "",
    "Ted is an open source terminal editor",
    "https://github.com/juliencombattelli/ted",
    "",
    "hit  CTRL+Q     to quit",
    "hit  CTRL+S     to save",
    // TODO format keybinds depending on current config
};

[[nodiscard]]
static bool can_draw_welcome_message()
{
    static constexpr auto project_length
        = [](const std::string_view& str) { return str.length(); };

    static constexpr size_t longest_line_len
        = std::ranges::max_element(welcome_message, {}, project_length)
              ->length();

    static constexpr size_t line_count = size(welcome_message);

    return longest_line_len < editor::get_screen_cols() - 1 // EOB char
        && line_count < editor::get_screen_rows();
}

[[nodiscard]]
static bool should_draw_welcome_message(size_t current_row)
{
    if (!can_draw_welcome_message()) {
        return false;
    }

    if (editor::state.viewed_file->lines.size() > 1) {
        return false;
    }

    if (editor::state.viewed_file->lines[0].length() > 2) {
        return false;
    }

    size_t start_line = (editor::get_screen_rows() - size(welcome_message)) / 2;
    size_t end_line = (editor::get_screen_rows() + size(welcome_message)) / 2;
    return start_line <= current_row && current_row <= end_line;
}

static void draw_welcome_message(size_t welcome_message_line)
{
    if (welcome_message_line >= size(welcome_message)) {
        return;
    }
    std::string line = std::format(
        "{:^{}}",
        welcome_message[welcome_message_line],
        editor::get_screen_cols() - 1);
    editor::screen_buffer_append(line.c_str());
}

static void draw_eob_chars(size_t eob_row)
{
    char eob_char = editor::state.eob_char;
    size_t welcome_message_line = 0;
    for (size_t row = eob_row; row < editor::get_screen_rows() - 1; row++) {
        editor::screen_buffer_append(eob_char);
        if (should_draw_welcome_message(row)) {
            draw_welcome_message(welcome_message_line++);
        }
        term::erase_line();
        editor::screen_buffer_append("\r\n");
    }
    editor::screen_buffer_append(eob_char);
    term::erase_line();
}

static void refresh_screen()
{
    handle_resize();

    term::cursor_hide();
    // TODO is this really needed?
    term::cursor_home();

    size_t eob_row = 0;
    if (editor::state.viewed_file != nullptr) {
        editor::File& file = *editor::state.viewed_file;
        for (const auto& line : file.lines) {
            editor::screen_buffer_append(line.c_str());
            editor::screen_buffer_append("\r\n");
        }
        eob_row = file.lines.size();
    }

    draw_eob_chars(eob_row);

    term::cursor_move(editor::state.cursor_row, editor::state.cursor_col);
    term::cursor_show();

    term::write_screen_buffer();
}

void start()
{
    while (true) {
        refresh_screen();
        Key::Code keycode = read_key();
        process_key(keycode);
    }
}

} // namespace ted::tui
