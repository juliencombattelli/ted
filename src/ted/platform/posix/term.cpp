#include <ted/os.hpp>
#include <ted/term.hpp>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>

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
    if (std::atexit(disable_raw_mode) != 0) {
        os::exit_err("std::atexit() failed");
    }
    termios raw = original_termios;
    raw.c_iflag &= ~(IXON | ICRNL | ISTRIP | INPCK | BRKINT);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(IEXTEN | ECHO | ICANON | ISIG);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0) {
        os::exit_err("tcsetattr() failed");
    }
}

void init()
{
    enable_raw_mode();
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

} // namespace ted::term
