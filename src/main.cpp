#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/tui.hpp>

#include <cctype>
#include <cstdio>

namespace ted {

} // namespace ted

int main(int /*argc*/, char* /*argv*/[])
{
    if (!ted::os::isatty(stdin) || !ted::os::isatty(stdout)) {
        std::fprintf(stderr, "not a tty\n");
        return 1;
    }

    ted::editor::State editor_state {};
    ted::editor::init(editor_state);

    ted::tui::start(editor_state);
}
