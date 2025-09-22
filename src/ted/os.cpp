#include <ted/os.hpp>
#include <ted/term.hpp>

#include <cassert>
#include <cstdio>
#include <cstdlib>

namespace ted::os {

void exit_ok()
{
    term::clear();
    std::exit(0);
}

void exit_err(const char* msg, int status)
{
    assert(status != 0);
    term::clear();
    std::perror(msg);
    std::exit(status);
}

} // namespace ted::os
