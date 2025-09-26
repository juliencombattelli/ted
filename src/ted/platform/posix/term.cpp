#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

#include <fcntl.h>
#include <signal.h> // NOLINT(*deprecated-headers*): sigaction is not standard C++
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ted::term {

static struct {
    int stdin_fd;
    int stdin_flags;
    int stdout_fd;
    termios stdout_initial_termios;
    bool terminal_resized;
} state {
    .stdin_fd = STDIN_FILENO,
    .stdin_flags = 0,
    .stdout_fd = STDOUT_FILENO,
    .stdout_initial_termios = {},
    .terminal_resized = false,
};

static void disable_raw_mode()
{
    // Restore the original terminal mode
    if (tcsetattr(state.stdout_fd, TCSANOW, &state.stdout_initial_termios)
        != 0) {
        os::exit_err("tcsetattr() failed");
    }
}

static void enable_raw_mode()
{
    // Store the stdin flags so we can more easily toggle `O_NONBLOCK` later on.
    if (int stdin_flags = fcntl(state.stdin_fd, F_GETFL); stdin_flags == -1) {
        os::exit_err("fcntl(F_GETFL) failed");
    } else {
        state.stdin_flags = stdin_flags;
    }

    // Set state.terminal_resized to true whenever we get SIGWINCH
    struct sigaction action {};
    action.sa_handler = [](int /*signal*/) { state.terminal_resized = true; };
    if (sigaction(SIGWINCH, &action, nullptr) != 0) {
        os::exit_err("sigaction(SIGWINCH) failed");
    }

    // Get the original termios
    termios raw {};
    if (tcgetattr(state.stdout_fd, &raw) != 0) {
        os::exit_err("tcgetattr() failed");
    }
    state.stdout_initial_termios = raw;
    // clang-tidy: the `0U |` trick is used to silence *signed-bitwise*
    raw.c_iflag &= ~(
        0U //
        | BRKINT // Do not signal interrupt on break
        | INPCK // Disable input parity checking
        | ISTRIP // Disable stripping of eighth bit
        | INLCR // Disable mapping of NL to CR on input
        | IGNCR // Disable ignoring CR on input
        | ICRNL // Disable mapping of CR to NL on input
        | IXON // Disable software flow control
    );
    raw.c_oflag &= ~(
        0U //
        | OPOST // Disable output processing
    );
    raw.c_cflag &= ~(
        0U //
        | CSIZE // Reset character size mask
        | PARENB // Disable parity generation
    );
    // Set character size back to 8 bits
    raw.c_cflag |= unsigned(CS8);
    raw.c_lflag &= ~(
        0U //
        | ISIG // Disable signal generation (SIGINT, SIGTSTP, SIGQUIT)
        | ICANON // Disable canonical mode (line buffering)
        | ECHO // Disable echoing of input characters
        | ECHONL // Disable echoing of NL
        | IEXTEN // Disable extended input processing (e.g. Ctrl-V)
    );

    // Set the terminal to raw mode
    if (tcsetattr(state.stdout_fd, TCSANOW, &raw) != 0) {
        os::exit_err("tcsetattr() failed");
    }
}

static void deinit()
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

bool read_key(uint8_t& byte)
{
    if (::read(STDIN_FILENO, &byte, 1) == EOF) {
        if (errno == EAGAIN) {
            return false; // timeout, nothing to read
        }
        if (errno == EINTR && state.terminal_resized) {
            state.terminal_resized = false;
            tui::handle_resize();
            return true; // interruption, break the caller read loop
        }
        os::exit_err("read() failed");
    }
    return true;
}

void print_n(const void* buffer, size_t size)
{
    // TODO handle error
    (void)::write(STDOUT_FILENO, buffer, size);
}

void print_cstr(const char* str)
{
    print_n(str, std::strlen(str));
}

} // namespace ted::term
