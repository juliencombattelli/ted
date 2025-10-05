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

struct SubstrResult {
    std::string_view substr;
    size_t byte_start; // only for test/debug
    size_t byte_len; // only for test/debug
    // whether the substr cuts a wide char at start or end
    bool cut_at_start;
    bool cut_at_end;
};
SubstrResult substr(State* state, std::string_view s, size_t pos, size_t n);

} // namespace ted::utf8

#endif // TED_UTF8_HPP_
