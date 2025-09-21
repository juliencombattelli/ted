#include <ted/os.hpp>

#include <cstdio>
#include <cstdlib>

namespace ted::os {

void exit_ok()
{
    std::exit(0);
}

void exit_err(const char* msg, int status)
{
    std::perror(msg);
    std::exit(status);
}

} // namespace ted::os
