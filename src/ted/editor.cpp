#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

namespace ted::editor {

void init(State& state)
{
    state.eob_char = '~';
}

} // namespace ted::editor