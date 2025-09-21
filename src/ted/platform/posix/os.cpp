#include <ted/os.hpp>

#include <cstdlib>
#include <unistd.h>

namespace ted::os {

bool isatty(FILE* stream)
{
    return ::isatty(fileno(stream)) == 1;
}

} // namespace ted::os
