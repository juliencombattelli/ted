#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif

#include <cstdio>
#include <unistd.h>

#include <platform/term.hpp>

bool term_isatty(FILE* stream)
{
    return isatty(fileno(stream)) == 1;
}
