#ifndef TED_EDITOR_HPP_
#define TED_EDITOR_HPP_

#include <cstdlib>
#include <string>

namespace ted::editor {

struct State {
    std::string screen_buffer;
    size_t screen_rows;
    size_t screen_cols;
    char eob_char;
};

extern State state;

void init(size_t rows, size_t column);

void screen_buffer_append(char c);
void screen_buffer_append(const char* s);

} // namespace ted::editor

#endif // TED_EDITOR_HPP_