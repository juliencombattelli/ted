#ifndef TED_TERM_HPP_
#define TED_TERM_HPP_

#include <cstdint>
#include <cstdlib>

namespace ted::term {

void init();

bool get_size(size_t& rows, size_t& columns);

void cursor_move(size_t row, size_t col);
void cursor_home();
void cursor_show();
void cursor_hide();

enum class EraseLineMode : uint8_t {
    RightOfCursor = 0,
    LeftOfCursor = 1,
    AllLine = 2,
};
void erase_line(EraseLineMode mode = EraseLineMode::AllLine);

enum class ClearMode : uint8_t {
    CursorToTop = 0,
    CursorToBottom = 1,
    AllScreen = 2,
};
void clear(ClearMode mode = ClearMode::AllScreen);

[[nodiscard]]
constexpr char key_ctrl(char key)
{
    return key & 0x1f;
}

} // namespace ted::term

#endif // TED_TERM_HPP_
