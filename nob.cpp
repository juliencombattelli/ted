#define NOB_IMPLEMENTATION
#include "src-build/external/nob.hpp"

#include <array>
#include <filesystem>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#if NOB_COMPILER_MSVC
#error "MSVC compiler is not supported yet"
#endif

static void create_output_file_dir_recursively(const std::string& output_file)
{
    nob::logf_debug("Creating build directory `%s`", output_file.c_str());
    std::filesystem::path output_file_path(output_file);
    std::error_code error;
    bool created = std::filesystem::create_directories(
        output_file_path.parent_path(),
        error);
    if (!created && error) {
        nob::logf_error(
            "Unable to create directory `%s`: %s (OS code %u)",
            output_file.c_str(),
            error.message().c_str(),
            error.value());
        std::exit(1);
    }
}

static constexpr std::array nob_input_files = {
    "nob.cpp",
};

#if NOB_COMPILER_MSVC
#define PLATFORM_DIR "platform/win32"
#else // assume POSIX-compatible platform
#define PLATFORM_DIR "platform/posix"
#endif

#define SRC_DIR "src"
#define BUILD_DIR "build"

static constexpr std::array ted_input_files = {
    SRC_DIR "/main.cpp",
    SRC_DIR "/" PLATFORM_DIR "/term.cpp",
};

#define OUTPUT_BINARY BUILD_DIR "/ted"

#define COMPDB_FILE BUILD_DIR "/" NOB_COMPDB_FILE

#define LOG_COMPILING "Building CXX object "
#define LOG_LINKING "Linking CXX executable "

nob::Cmd build_nob_cmd(const char* input)
{
    std::string output = nob::into_object_file(input, BUILD_DIR);
    nob::Cmd cmd;
    nob::cmd_append_cxx_compiler(cmd);
    nob::cmd_append_output(cmd, output);
    nob::cmd_append_input(cmd, input);
    return cmd;
}

nob::Cmd build_cmd(const char* input)
{
    std::string output = nob::into_object_file(input, BUILD_DIR);
    create_output_file_dir_recursively(output);
    nob::Cmd cmd;
    cmd.name
        = nob::str_printf("\e[32m" LOG_COMPILING "%s\e[0m", output.c_str());
#ifdef NOB_COMPILER_MSVC
#else
    nob::cmd_append_cxx_compiler(cmd);
    nob::cmd_append(cmd, "-Wall");
    nob::cmd_append(cmd, "-Wextra");
    nob::cmd_append(cmd, "-pedantic");
    nob::cmd_append(cmd, "-std=c++23");
    nob::cmd_append(cmd, "-c");
#endif
    nob::cmd_append_output(cmd, output);
    nob::cmd_append_input(cmd, input);
    nob::cmd_append(cmd, "-I" SRC_DIR);
    return cmd;
}

nob::Cmd link_cmd(const char* output, const nob::CmdGroup& build_cmd_group)
{
    nob::Cmd cmd;
    cmd.name = nob::str_printf("\e[1;32m" LOG_LINKING "%s\e[0m", output);
    nob::cmd_append_cxx_compiler(cmd);
    nob::cmd_append_output(cmd, output);
    for (const nob::Cmd& build_cmd : build_cmd_group) {
        // Take object files to link them
        nob::cmd_append_inputs(cmd, build_cmd.outputs);
    }
    return cmd;
}

int main(int argc, char** argv)
{
    // TODO Add --trace and --log-level cli arguments
    NOB_GO_REBUILD_YOURSELF(argc, argv);

    nob::logf_debug("Creating build directory `" BUILD_DIR "`");
    std::error_code error;
    if (!std::filesystem::create_directory(BUILD_DIR, error) && error) {
        nob::logf_error(
            "Unable to create build directory `" BUILD_DIR
            "`: %s ( OS code %u)",
            error.message().c_str(),
            error.value());
    }

    std::vector<nob::CmdGroup> cmd_groups(2);

    {
        nob::CmdGroup& nob_cmds = cmd_groups[0];
        for (const auto& input : nob_input_files) {
            nob_cmds.push_back(build_nob_cmd(input));
        }
    }

    nob::CmdGroup& ted_cmds = cmd_groups[1];
    for (const auto& input : ted_input_files) {
        ted_cmds.push_back(build_cmd(input));
    }

    nob::logf_debug("Generating compdb in `%s`", COMPDB_FILE);
    if (!nob::compdb_dump(cmd_groups, COMPDB_FILE)) {
        nob::logf_warning("Unable to generate compdb");
    }

    ted_cmds.push_back(link_cmd(OUTPUT_BINARY, ted_cmds));

    const size_t cmd_count = ted_cmds.size();
    size_t cmd_done = 1;
    for (const nob::Cmd& cmd : ted_cmds) {
        size_t progress = cmd_done * 100 / cmd_count;
        if (!cmd.name.empty()) {
            nob::logf_info("[%3u%] %s", progress, cmd.name.c_str());
        } else {
            nob::logf_info(
                "[%3u%] Running `%s`",
                progress,
                nob::cmd_render(cmd.command_line).c_str());
        }
        if (!nob::cmd_run(cmd.command_line)) {
            return 1;
        }
        cmd_done += 1;
    }
    nob::logf_info("Build completed successfully!");

    return 0;
}
