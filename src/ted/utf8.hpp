#ifndef TED_UTF8_HPP_
#define TED_UTF8_HPP_

#include <cstdlib>
#include <string_view>

namespace ted::utf8 {

void init();
void deinit();

struct State;

State* create();
void destroy(State*);

size_t strlen(State* state, std::string_view s);
std::string_view substr(State* state, std::string_view s, size_t pos, size_t n);

} // namespace ted::utf8

#endif // TED_UTF8_HPP_
