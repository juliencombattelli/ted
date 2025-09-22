#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

namespace ted::editor {

void init(State& state, size_t rows, size_t cols)
{
    state.screen_rows = rows;
    state.screen_cols = cols;
    state.eob_char = '~';
    state.screen_buffer.reserve(state.screen_rows * state.screen_cols);
}

} // namespace ted::editor