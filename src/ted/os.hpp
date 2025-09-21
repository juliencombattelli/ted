#ifndef TED_OS_HPP_
#define TED_OS_HPP_

#include <cstdio>
#include <cstdlib>

namespace ted::os {

[[nodiscard]]
bool isatty(FILE* stream);

[[noreturn]]
void exit_ok();
[[noreturn]]
void exit_err(const char* msg, int status = 1);

} // namespace ted::os

#endif // TED_OS_HPP_
