#include <ted/os.hpp>
#include <ted/term.hpp>

#include <cctype>
#include <cstdio>
#include <utility>

namespace ted {

[[nodiscard]] int read_key()
{
    int c = std::fgetc(stdin);
    if (c == EOF) {
        if (std::ferror(stdin)) {
            ted::os::exit_err("fread() failed");
        }
        if (std::feof(stdin)) {
            ted::os::exit_err("fread() end-of-file", 0);
        }
        std::unreachable();
    }
    return c;
}

void process_key(int c)
{
    if (std::iscntrl(c)) {
        std::printf("%d\r\n", c);
    } else {
        std::printf("%d ('%c')\r\n", c, c);
    }

    switch (c) {
    case ted::term::key_ctrl('q'):
        ted::os::exit_ok();
        break;
    case ted::term::key_ctrl('c'):
        ted::term::clear();
        break;
    case '\r':
        std::putchar('\r');
        std::putchar('\n');
    default:
        std::putchar(c);
    }
}

} // namespace ted

int main(int /*argc*/, char* /*argv*/[])
{
    if (!ted::os::isatty(stdin) || !ted::os::isatty(stdout)) {
        std::fprintf(stderr, "not a tty\n");
        return 1;
    }

    ted::term::init();

    unsigned char c = '\0';
    while (true) {
        char c = ted::read_key();
        ted::process_key(c);
    }
}
