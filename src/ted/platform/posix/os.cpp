#include <ted/os.hpp>

#include <cstdlib>
#include <unistd.h>

namespace ted::os {

bool isatty(int fd)
{
    return ::isatty(fd) == 1;
}

} // namespace ted::os
