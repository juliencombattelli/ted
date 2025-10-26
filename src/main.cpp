#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/term.hpp>
#include <ted/tui.hpp>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <span>
#include <string_view>

struct Arguments {
    std::vector<std::string> files;
    bool debug;
};

static void usage()
{
    // NOLINTNEXTLINE(*avoid-c-arrays*)
    static constexpr char usage_message[] = R"(Usage:
    ted [options] [file ...]

Options:
    --debug, -d     Enable debug information printing into stderr
    --help, -h      Print this help message
    --version, -v   Print version information
    --              All arguments after this will be interpreted as files to open
)";
    // TODO add -i, --interface {tui,gui}
    (void)std::fputs(usage_message, stdout);
}

static void version()
{
    (void)std::fputs("Ted v" TED_VERSION "\n", stderr);
}

static Arguments parse_arguments(std::span<char*> args)
{
    Arguments arguments {};
    bool swallow_remaining_as_files = false;
    for (std::string_view arg : args.subspan(1)) {
        if (arg.starts_with('-') && !swallow_remaining_as_files) {
            if (arg == "--") {
                swallow_remaining_as_files = true;
            } else if (arg == "-d" || arg == "--debug") {
                arguments.debug = true;
            } else if (arg == "-h" || arg == "--help") {
                usage();
                std::exit(EXIT_SUCCESS); // NOLINT(*concurrency*)
            } else if (arg == "-v" || arg == "--version") {
                version();
                std::exit(EXIT_SUCCESS); // NOLINT(*concurrency*)
            } else {
                usage();
                std::exit(EXIT_FAILURE); // NOLINT(*concurrency*)
            }
        } else {
            arguments.files.emplace_back(arg);
        }
    }
    return arguments;
}

int main(int argc, char* argv[])
{
    TED_ASSERT(argc > 0);
    Arguments args
        = parse_arguments(std::span(argv, static_cast<size_t>(argc)));

    if (args.debug) {
        ted::os::print_source_location_at_exit(true);
    }

    ted::editor::init();
    if (args.debug) {
        ted::editor::dump_state_open("ted.dumpstate");
    }
    ted::term::init();
    ted::tui::init();
    if (args.files.size() == 0) {
        ted::editor::open_new_file();
    } else {
        for (const auto& filepath : args.files) {
            ted::editor::open_file(filepath.c_str());
        }
    }
    ted::tui::start();
    ted::term::deinit();
    if (args.debug) {
        ted::editor::dump_state_close();
    }
    ted::editor::deinit();
}
