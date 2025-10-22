#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>
#include <ted/utf8.hpp>

#include <algorithm>
#include <fstream>
#include <functional>
#include <string>
#include <string_view>

namespace ted::editor {

uint8_t rendered_char_column_width(const ted::utf8::Char& ch)
{
    auto sv = ch.to_string_view();
    if (sv == "\t") {
        return get_tab_width();
    }
    return ch.column;
}

size_t column_count(std::string_view str)
{
    size_t col_count = 0;
    utf8::Chars chars = utf8::chars(str, state.utf8_chars_iterator_state);
    for (utf8::Char ch : chars) {
        col_count += ch.column;
    }

    return col_count;
}

size_t rendered_column_count(std::string_view str)
{
    size_t col_count = 0;
    utf8::Chars chars = utf8::chars(str, state.utf8_chars_iterator_state);
    for (utf8::Char ch : chars) {
        col_count += rendered_char_column_width(ch);
    }

    return col_count;
}

template<typename P>
static SubstrResult column_substr_impl(
    std::string_view str,
    size_t col_pos,
    size_t col_n,
    P&& column_projector)
{
    using sv = std::string_view;

    SubstrResult result {
        .substr {},
        .offset_from_start_char = 0,
        .cut_at_start = 0,
        .cut_at_end = 0,
    };

    const size_t col_end = col_pos + col_n;
    size_t current_col = 0;
    size_t byte_start = sv::npos;
    size_t byte_end = sv::npos;
    size_t last_included_col_end = 0;

    ted::utf8::Chars chars
        = ted::utf8::chars(str, state.utf8_chars_iterator_state);

    for (ted::utf8::Char ch : chars) {

        const uint8_t char_width
            = std::invoke(std::forward<P>(column_projector), ch);
        const size_t char_start_col = current_col;
        const size_t char_end_col = current_col + char_width;

        // Check if this character overlaps with the requested column range
        const bool overlaps_start = char_end_col > col_pos;
        const bool overlaps_end = char_start_col < col_end;

        if (overlaps_start && overlaps_end) {
            // This character is within or overlaps the requested range

            // Handle first character
            if (current_col < col_pos) {
                if (char_start_col < col_pos) {
                    // Character is partially cut at start
                    result.offset_from_start_char = col_pos - char_start_col;
                    result.cut_at_start
                        = char_width - result.offset_from_start_char;
                    last_included_col_end = col_pos;
                    // Skip this cut character, move to next
                    current_col = char_end_col;
                    continue;
                }

                // Character starts at or after col_pos (gap at start)
                result.cut_at_start = char_start_col - col_pos;
                result.offset_from_start_char = 0;
            }

            // Try to include character if it fits
            if (char_end_col > col_end) { // Character doesn't fit, stop here
                break;
            }
            if (byte_start == sv::npos) { // Character fits, include it
                byte_start = ch.start_byte;
            }

            byte_end = ch.next_byte;
            last_included_col_end = char_end_col;

        } else if (current_col >= col_end) {
            break; // We've passed the end of the requested range
        }
        current_col = char_end_col;
    }

    // Build the resulting substring
    if (byte_start != sv::npos && byte_end != sv::npos) {
        // Calculate cut_at_end: columns at the end of the range not covered
        if (last_included_col_end < col_end) {
            result.cut_at_end = col_end - last_included_col_end;
        }
        result.substr = str.substr(byte_start, byte_end - byte_start);
    }

    return result;
}

SubstrResult column_substr(std::string_view str, size_t col_pos, size_t col_n)
{
    return column_substr_impl(str, col_pos, col_n, &utf8::Char::column);
}

SubstrResult rendered_column_substr(
    std::string_view str,
    size_t col_pos,
    size_t col_n)
{
    return column_substr_impl(str, col_pos, col_n, &rendered_char_column_width);
}

// NOLINTNEXTLINE(*global*)
State state;

static struct {
    std::string buffer;
    std::fstream file;
} state_dumper; // NOLINT(*global*)

void dump_state_open(const char* filename)
{
    // Large enough to avoid allocation during dumping
    static constexpr size_t buffer_initial_alloc = 4096;

    state_dumper.buffer.reserve(buffer_initial_alloc);
    state_dumper.file.open(filename, std::fstream::app | std::fstream::ate);
    if (!state_dumper.file.is_open()) {
        os::exit_err_format("Cannot open file {}", filename);
    }

    state.config.debug_enabled = true;
}

void dump_state_close()
{
    state_dumper.file.close();
}

void dump_state()
{
    static size_t iteration = 0;
    state_dumper.buffer.clear();
    std::format_to(
        std::back_inserter(state_dumper.buffer),
        "it={}, "
        "scr={{r={},c={}}}, "
        "cur={{r={},c={}}}, "
        "curcoladj={}, "
        "vwp={{r={},c={}}}\n",
        iteration++,
        state.screen_size.rows,
        state.screen_size.cols,
        state.cursor_coord.row,
        state.cursor_coord.col,
        state.cursor_col_rendered,
        state.viewport_offset.row,
        state.viewport_offset.col);
    state_dumper.file << state_dumper.buffer << std::flush;
}

void init()
{
    utf8::init();
    // Preallocate the Unicode character iterator state object
    state.utf8_chars_iterator_state = utf8::create_chars_iterator_state();

    // Load default configuration
    state.config.eob_char = '~';
    state.config.tab_width = 4;
    state.config.tab_str = "› ";
    // TODO crashing if config is not done above, to be investigated
    set_tab_width(4);
    set_tab_string("› ");
    // Keymap is not initialized here as the default mapping could change
    // between a TUI or GUI mode
}

void deinit()
{
    utf8::destroy_state(state.utf8_chars_iterator_state);
    utf8::deinit();
}

void screen_buffer_append_char(char c)
{
    if (c == '\t') {
        state.screen_buffer.append(state.full_tab_string);
    } else {
        state.screen_buffer.push_back(c);
    }
}
void screen_buffer_append(const char* s)
{
    TED_ASSERT(s != nullptr);
    while (*s) {
        screen_buffer_append_char(*s++);
    }
}
void screen_buffer_append_n(const char* s, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        screen_buffer_append_char(s[i]);
    }
}
void screen_buffer_append_substr(const SubstrResult& substr)
{
    state.screen_buffer.append(substr.cut_at_start, ' ');
    screen_buffer_append_n(substr.substr.data(), substr.substr.length());
    state.screen_buffer.append(substr.cut_at_end, ' ');
}

void scroll()
{
    auto& viewport_row = state.viewport_offset.row;
    auto& viewport_col = state.viewport_offset.col;

    viewport_row = std::min(viewport_row, get_cursor_row());

    if (get_cursor_row() >= viewport_row + get_screen_rows()) {
        viewport_row = get_cursor_row() - get_screen_rows() + 1;
    }

    viewport_col = std::min(viewport_col, get_cursor_col());

    if (get_cursor_col() >= viewport_col + get_screen_cols()) {
        viewport_col = get_cursor_col() - get_screen_cols() + 1;
    }
}

static Line& get_cursor_text_line()
{
    TED_ASSERT(state.cursor_coord.row < state.viewed_file->lines.size());
    return state.viewed_file->lines[state.cursor_coord.row];
}

static void fixup_cursor_col()
{
    // Adjust cursor column position when switching line
    Line& cursor_line = get_cursor_text_line();
    size_t row_length = rendered_column_count(cursor_line);
    state.cursor_col_rendered
        = std::min(state.cursor_col_memorized, row_length);
    SubstrResult substr
        = rendered_column_substr(cursor_line, state.cursor_col_rendered, 0);
    state.cursor_col_rendered -= substr.offset_from_start_char;
    state.cursor_coord.col = state.cursor_col_rendered;
}

void cursor_up()
{
    if (state.cursor_coord.row > 0) {
        state.cursor_coord.row--;
    }
    fixup_cursor_col();
}
void cursor_down()
{
    if (state.cursor_coord.row < state.viewed_file->lines.size() - 1) {
        state.cursor_coord.row++;
    }
    fixup_cursor_col();
}
void cursor_left()
{
    Line& cursor_line = get_cursor_text_line();
    if (state.cursor_coord.col > 0) {
        state.cursor_coord.col--;
        if (column_substr(cursor_line, state.cursor_coord.col, 1).substr
            == "\t") {
            state.cursor_col_rendered -= get_tab_width();
        } else {
            state.cursor_col_rendered--;
        }
    } else if (state.cursor_coord.row > 0) {
        state.cursor_coord.row--;
        cursor_end_of_line();
    }
    state.cursor_col_memorized = state.cursor_col_rendered;
}
void cursor_right()
{
    Line& cursor_line = get_cursor_text_line();
    if (state.cursor_coord.col < column_count(cursor_line)) {
        if (column_substr(cursor_line, state.cursor_coord.col, 1).substr
            == "\t") {
            state.cursor_col_rendered += get_tab_width();
        } else {
            state.cursor_col_rendered++;
        }
        state.cursor_coord.col++;
    } else if (state.cursor_coord.row < state.viewed_file->lines.size() - 1) {
        state.cursor_coord.row++;
        cursor_start_of_line();
    }
    state.cursor_col_memorized = state.cursor_col_rendered;
}
void cursor_start_of_line()
{
    state.cursor_coord.col = 0;
    state.cursor_col_rendered = state.cursor_coord.col;
    state.cursor_col_memorized = state.cursor_coord.col;
}
void cursor_end_of_line()
{
    Line& cursor_line = get_cursor_text_line();
    state.cursor_coord.col = rendered_column_count(cursor_line);
    state.cursor_col_rendered = state.cursor_coord.col;
    state.cursor_col_memorized = state.cursor_coord.col;
}

/*void set_cursor_row(size_t row)
{
    state.cursor_coord.row = std::min(row, state.screen_size.rows - 1);
}
void set_cursor_row_top()
{
    state.cursor_coord.row = 0;
}
void set_cursor_row_bot()
{
    state.cursor_coord.row = state.screen_size.rows - 1;
}*/
size_t get_cursor_row()
{
    return state.cursor_coord.row;
}

/*void set_cursor_col(size_t col)
{
    state.cursor_coord.col = std::min(col, state.screen_size.cols - 1);
}
void set_cursor_col_left()
{
    state.cursor_coord.col = 0;
}
void set_cursor_col_right()
{
    state.cursor_coord.col = state.screen_size.cols - 1;
}*/
size_t get_cursor_col()
{
    return state.cursor_col_rendered;
}

static void set_screen_size(ScreenSize screen_size)
{
    state.screen_size = screen_size;
    // Reserve more than a screen to have spare space for wide chars (like tabs)
    state.screen_buffer.reserve(2 * screen_size.rows * screen_size.cols);
}
static ScreenSize get_screen_size()
{
    return state.screen_size;
}

void set_screen_rows(size_t rows)
{
    // Keep room for the status bar
    set_screen_size(ScreenSize { rows - 2, get_screen_cols() });
}
size_t get_screen_rows()
{
    return state.screen_size.rows;
}
void set_screen_cols(size_t cols)
{
    set_screen_size(ScreenSize { get_screen_rows(), cols });
}
size_t get_screen_cols()
{
    return state.screen_size.cols;
}

static void update_full_tab_string()
{
    // Limit state.config.tab_str to state.config.tab_width columns
    // TODO emit warning if column_count(tab_str) > tab_width
    // TODO emit error if tab_str is empty or tab_width == 0
    std::string_view tab_str
        = column_substr(state.config.tab_str, 0, state.config.tab_width).substr;
    state.full_tab_string = tab_str;
    // Pad remaining columns with the last char in tab_str
    uint8_t remaining = state.config.tab_width - column_count(tab_str);
    state.full_tab_string.append(remaining, tab_str.back());
}

void set_tab_width(uint8_t width)
{
    state.config.tab_width = width;
    update_full_tab_string();
}
uint8_t get_tab_width()
{
    return state.config.tab_width;
}

void set_tab_string(std::string_view s)
{
    state.config.tab_str = s;
    update_full_tab_string();
}
std::string_view get_tab_string()
{
    return state.config.tab_str;
}

void set_keymap(Key::Code keycode, KeyHandler* handler)
{
    state.config.keymap[keycode] = handler;
}

KeyHandler* get_keymap(Key::Code keycode)
{
    return state.config.keymap[keycode];
}

void open_new_file()
{
    state.viewed_file = &state.opened_files.emplace_back();
    state.viewed_file->name = "[No Name]";
    state.viewed_file->lines.emplace_back("");
    state.viewed_file->longest_line_size = 0;
}
void open_file(const char* path)
{
    state.viewed_file = &state.opened_files.emplace_back();

    state.viewed_file->name = path;

    std::fstream file(path);
    if (!file.is_open()) {
        os::exit_err_format("Cannot open file {}", path);
    }

    for (std::string line; std::getline(file, line);) {
        state.viewed_file->lines.emplace_back(std::move(line));
    }

    state.viewed_file->longest_line_size = column_count(*std::max_element(
        state.viewed_file->lines.begin(),
        state.viewed_file->lines.end(),
        [](const Line& s1, const Line& s2) {
            return s1.length() < s2.length();
        }));
}

} // namespace ted::editor
