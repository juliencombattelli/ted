#include <cstdlib>
#include <string_view>
#include <ted/editor.hpp>
#include <ted/os.hpp>
#include <ted/tui.hpp>

#include <cctype>
#include <cstdio>
#include <span>

struct Arguments {
    std::vector<std::string> files;
    bool debug;
};

static void usage()
{
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
    Arguments arguments;
    bool swallow_remaining_as_files = false;
    for (std::string_view arg : args.subspan(1)) {
        if (arg.starts_with('-') && !swallow_remaining_as_files) {
            if (arg == "--") {
                swallow_remaining_as_files = true;
            } else if (arg == "-d" || arg == "--debug") {
                arguments.debug = true;
            } else if (arg == "-h" || arg == "--help") {
                usage();
                std::exit(EXIT_SUCCESS);
            } else if (arg == "-v" || arg == "--version") {
                version();
                std::exit(EXIT_SUCCESS);
            } else {
                usage();
                std::exit(EXIT_FAILURE);
            }
        } else {
            arguments.files.emplace_back(arg);
        }
    }
    return arguments;
}

int main(int argc, char* argv[])
{
    Arguments args = parse_arguments(std::span(argv, argc));

    if (args.debug) {
        ted::os::print_source_location_at_exit(true);
    }

    if (!ted::os::isatty(stdin) || !ted::os::isatty(stdout)) {
        std::fprintf(stderr, "not a tty\n");
        return 1;
    }

    ted::editor::init();
    ted::tui::init();
    if (args.files.size() == 0) {
        ted::editor::open_new_file();
    } else {
        for (const auto& filepath : args.files) {
            ted::editor::open_file(filepath.c_str());
        }
    }
    ted::tui::start();
}
