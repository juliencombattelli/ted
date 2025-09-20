#include <platform/term.hpp>

int main(int /*argc*/, char* /*argv*/[])
{
    if (!term_isatty(stdin) || !term_isatty(stdout)) {
        fprintf(stderr, "not a tty\n");
        return 1;
    }
}
