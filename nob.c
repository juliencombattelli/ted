#define NOB_IMPLEMENTATION
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "external/nob.h/nob.h"

#if defined(_MSC_VER) && !defined(__clang__)
#error "Win32 platform is not supported yet"
#endif

typedef struct {
    const char* input;
    const char* output;
    Nob_Cmd cmd;
} Cmd;

typedef struct {
    Cmd* items;
    size_t count;
    size_t capacity;
} Cmds;

typedef struct {
    Nob_String_Builder sb;
} CompDb;

static void compdb_start(CompDb* compdb)
{
    nob_sb_append_cstr(&compdb->sb, "[");
}

static void compdb_add_entry(CompDb* compdb, Cmd* cmd)
{
    const char* cwd = nob_get_current_dir_temp();
    Nob_String_Builder command_str = { 0 };
    nob_cmd_render(cmd->cmd, &command_str);
    nob_sb_append_null(&command_str);
    nob_sb_appendf(
        &compdb->sb,
        "\n"
        "  {\n"
        "    \"directory\": \"%s\",\n"
        "    \"command\": \"%s\",\n"
        "    \"file\": \"%s\",\n"
        "    \"output\": \"%s\"\n"
        "  }",
        cwd,
        command_str.items,
        cmd->input,
        cmd->output);
    nob_sb_free(command_str);
}

static void compdb_entry_continue(CompDb* compdb)
{
    nob_sb_append_cstr(&compdb->sb, ",");
}

static void compdb_end(CompDb* compdb)
{
    nob_sb_append_cstr(&compdb->sb, "\n]\n");
}

static void compdb_free(CompDb* compdb)
{
    nob_sb_free(compdb->sb);
}

static bool compdb_dump_to_file(
    Cmds** cmds_groups,
    size_t cmds_group_count,
    const char* file)
{
    bool result = true;
    CompDb compdb = { 0 };
    // 1MiB compile_commands.json should be enough to start with
    nob_da_reserve(&compdb.sb, 1024ULL * 1024);
    {
        compdb_start(&compdb);
        {
            for (size_t group = 0; group < cmds_group_count; group++) {
                Cmds* cmds = cmds_groups[group];
                for (size_t i = 0; i < cmds->count; i++) {
                    Cmd* cmd = &cmds->items[i];
                    compdb_add_entry(&compdb, cmd);
                    if (group < cmds_group_count - 1 || i < cmds->count - 1) {
                        compdb_entry_continue(&compdb);
                    }
                }
            }
        }
        compdb_end(&compdb);
    }
    result = nob_write_entire_file(file, compdb.sb.items, compdb.sb.count);
    compdb_free(&compdb);
    return result;
}

static Nob_String_View nob_sv_chop_by_delim_right(
    Nob_String_View* sv,
    char delim)
{
    NOB_ASSERT(sv->count < SIZE_MAX / 2); // Ensure sv->count fits in ssize_t
    ssize_t i = (ssize_t)sv->count;
    while (i > 0 && sv->data[i] != delim) {
        i -= 1;
    }

    Nob_String_View left_part = nob_sv_from_parts(sv->data, i);

    if (i < sv->count) {
        sv->count -= i + 1;
        sv->data += i + 1;
    } else {
        sv->count -= i;
        sv->data += i;
    }

    return left_part;
}

static void nob_sb_append_sv(Nob_String_Builder* sb, Nob_String_View sv)
{
    nob_sb_append_buf(sb, sv.data, sv.count);
}

static void output_file(
    const char* input,
    const char* build_dir,
    Nob_String_Builder* output)
{
    Nob_String_View input_sv = nob_sv_from_cstr(input);
    Nob_String_View filepath_without_ext
        = nob_sv_chop_by_delim_right(&input_sv, '.');
    nob_sb_appendf(
        output,
        "%s/" SV_Fmt ".o",
        build_dir,
        SV_Arg(filepath_without_ext));
    nob_sb_append_null(output);
}

static void create_output_file_dir_recursively(const char* output_file)
{
    Nob_String_View output_sv = nob_sv_from_cstr(output_file);
    Nob_String_View filedir = nob_sv_chop_by_delim_right(&output_sv, '/');
    Nob_String_Builder path = { 0 };
    while (filedir.count > 0) {
        Nob_String_View dir_component = nob_sv_chop_by_delim(&filedir, '/');
        nob_sb_append_sv(&path, dir_component);
        nob_sb_append_cstr(&path, "/");
        if (!nob_mkdir_if_not_exists(nob_temp_sv_to_cstr(nob_sb_to_sv(path)))) {
            exit(1);
        }
    }
    nob_sb_free(path);
}

static const char* const nob_input_files[] = {
    "nob.c",
};
const size_t nob_input_files_count = NOB_ARRAY_LEN(nob_input_files);

#if defined(_MSC_VER) && !defined(__clang__)
#define PLATFORM_DIR "platform/win32"
#else
#define PLATFORM_DIR "platform/posix"
#endif

#define SRC_DIR "src"
#define BUILD_DIR "build"

static const char* const ted_input_files[] = {
    SRC_DIR "/main.cpp",
    SRC_DIR "/" PLATFORM_DIR "/term.cpp",
};
const size_t ted_input_files_count = NOB_ARRAY_LEN(ted_input_files);

#define OUTPUT_BINARY BUILD_DIR "/ted"

#define CXXFLAGS "-Wall", "-Wextra",

Cmd build_nob_cmd(const char* input)
{
    Nob_String_Builder output_sb = { 0 };
    output_file(input, BUILD_DIR, &output_sb);
    Cmd cmd = {
        .input = input,
        .output = output_sb.items,
    };
    nob_cc(&cmd.cmd);
    nob_cc_output(&cmd.cmd, output_sb.items);
    nob_cc_inputs(&cmd.cmd, input);
    return cmd;
}

Cmd build_cmd(const char* input)
{
    Nob_String_Builder output_sb = { 0 };
    output_file(input, BUILD_DIR, &output_sb);
    create_output_file_dir_recursively(output_sb.items);
    Cmd cmd = {
        .input = input,
        .output = output_sb.items,
    };
#if defined(_MSC_VER) && !defined(__clang__)
#else
    nob_cmd_append(&cmd.cmd, "c++");
    nob_cmd_append(&cmd.cmd, "-Wall");
    nob_cmd_append(&cmd.cmd, "-Wextra");
    nob_cmd_append(&cmd.cmd, "-pedantic");
    nob_cmd_append(&cmd.cmd, "-std=c++23");
#endif
    nob_cc_output(&cmd.cmd, output_sb.items);
    nob_cmd_append(&cmd.cmd, "-c", input);
    nob_cmd_append(&cmd.cmd, "-I" SRC_DIR);
    return cmd;
}

Cmd link_cmd(const char* output, Cmds* build_cmds)
{
    Cmd cmd = {
        .input = NULL, // not used
        .output = output,
    };
    nob_cmd_append(&cmd.cmd, "c++");
    nob_cc_output(&cmd.cmd, output);
    for (size_t i = 0; i < build_cmds->count; i++) {
        nob_cc_inputs(&cmd.cmd, build_cmds->items[i].output);
    }
    return cmd;
}

int main(int argc, char** argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    nob_log(NOB_INFO, "creating build directory `" BUILD_DIR "`");
    if (!nob_mkdir_if_not_exists(BUILD_DIR)) {
        nob_log(NOB_ERROR, "unable to create build directory `" BUILD_DIR "`");
    }

    Cmds nob_cmds = { 0 };
    for (size_t i = 0; i < nob_input_files_count; i++) {
        const char* input = nob_input_files[i];
        Cmd cmd = build_nob_cmd(input);
        nob_da_append(&nob_cmds, cmd);
    }

    Cmds ted_cmds = { 0 };
    for (size_t i = 0; i < ted_input_files_count; i++) {
        const char* input = ted_input_files[i];
        Cmd cmd = build_cmd(input);
        nob_da_append(&ted_cmds, cmd);
    }

    Cmds* cmds_groups[] = { &nob_cmds, &ted_cmds };
    const size_t cmds_group_count = NOB_ARRAY_LEN(cmds_groups);

    nob_log(
        NOB_INFO,
        "generating compdb at `%s`",
        BUILD_DIR "/compile_commands.json");
    if (!compdb_dump_to_file(
            cmds_groups,
            NOB_ARRAY_LEN(cmds_groups),
            BUILD_DIR "/compile_commands.json")) {
        nob_log(NOB_WARNING, "unable to generate compdb");
    }

    Cmd link = link_cmd(OUTPUT_BINARY, &ted_cmds);
    nob_da_append(&ted_cmds, link);

    for (size_t i = 0; i < ted_cmds.count; i++) {
        if (!nob_cmd_run(&ted_cmds.items[i].cmd)) {
            return 1;
        }
    }

    // TODO iterate on cmds to free
    for (size_t group = 0; group < cmds_group_count; group++) {
        Cmds* cmds = cmds_groups[group];
        for (size_t i = 0; i < cmds->count; i++) {
            nob_da_free(cmds->items[i].cmd);
        }
        nob_da_free(*cmds);
    }

    return 0;
}
