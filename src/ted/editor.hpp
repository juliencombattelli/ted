#ifndef TED_EDITOR_HPP_
#define TED_EDITOR_HPP_

#include <cstdlib>

namespace ted::editor {

struct State {
    size_t screen_rows;
    size_t screen_cols;
    char eob_char;
};

void init(State& state);

} // namespace ted::editor

#endif // TED_EDITOR_HPP_