#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

namespace ted::editor {

State state;

void init(size_t rows, size_t cols)
{
    state.screen_rows = rows;
    state.screen_cols = cols;
    state.eob_char = '~';
    state.screen_buffer.reserve(state.screen_rows * state.screen_cols);
}

void screen_buffer_append(char c)
{
    state.screen_buffer.push_back(c);
}
void screen_buffer_append(const char* s)
{
    state.screen_buffer.append(s);
}

} // namespace ted::editor