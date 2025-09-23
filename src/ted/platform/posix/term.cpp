#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace ted::term {

static termios original_termios;

static void disable_raw_mode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) != 0) {
        os::exit_err("tcsetattr() failed");
    }
}

static void enable_raw_mode()
{
    if (tcgetattr(STDIN_FILENO, &original_termios) != 0) {
        os::exit_err("tcgetattr() failed");
    }
    termios raw = original_termios;
    raw.c_iflag &= ~(IXON | ICRNL | ISTRIP | INPCK | BRKINT);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(IEXTEN | ECHO | ICANON | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0) {
        os::exit_err("tcsetattr() failed");
    }
}

void deinit()
{
    disable_raw_mode();
    enter_main_screen_buffer();
}

void init()
{
    enter_alternate_screen_buffer();
    enable_raw_mode();

    os::at_exit(os::Ring::_1, deinit);
}

bool get_size(size_t& rows, size_t& columns)
{
    winsize ws {};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return false;
    }
    rows = ws.ws_row;
    columns = ws.ws_col;
    return true;
}

[[nodiscard]]
static Key::Code read_escape_sequence()
{
    char seq[3];

    if (::read(STDIN_FILENO, &seq[0], 1) != 1) {
        return Key::Code { '\e' };
    }
    if (::read(STDIN_FILENO, &seq[1], 1) != 1) {
        return Key::Code { '\e' };
    }
    if (seq[0] == '[') {
        switch (seq[1]) {
        case 'A':
            return Key::Code::Up;
        case 'B':
            return Key::Code::Down;
        case 'C':
            return Key::Code::Right;
        case 'D':
            return Key::Code::Left;
        default:
            break;
        }
    }
    return Key::Code { '\e' };
}

Key::Code read_key()
{
    ssize_t n = -1;
    uint8_t c = 0;
    while ((n = ::read(STDIN_FILENO, &c, 1)) != 1) {
        if (n == -1 && errno != EAGAIN) {
            os::exit_err("read() failed");
        }
    }
    if (c == '\e') {
        return read_escape_sequence();
    }
    return Key::Code { c };
}

void write_screen_buffer()
{
    std::string& screen_buffer = editor::state.screen_buffer;
    // TODO handle error
    (void)::write(STDOUT_FILENO, screen_buffer.data(), screen_buffer.length());
    screen_buffer.clear();
}

} // namespace ted::term
