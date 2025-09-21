/*
MIT License

Copyright (c) 2025 Julien Combattelli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef NOB_HPP_
#define NOB_HPP_

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

// User-provided macro to change the namespace name in case of conflict.
// Defaults to `nob`.
#ifndef NOB_NAMESPACE
#define NOB_NAMESPACE nob
#endif // NOB_NAMESPACE

// User-provided macro applied to all functions at namespace scope.
// Goes before each and every function declarations and definitions.
// Useful to `#define NOB_DEF static inline` if your source code is a single
// file and you want the compiler to remove unused functions.
// Default to empty.
#ifndef NOB_DEF
#define NOB_DEF
#endif // NOB_DEF

// Utility to expand a macro and stringify its value.
#define NOB_STRINGIFY_VALUE_OF(x) NOB_STRINGIFY(x)
#define NOB_STRINGIFY(x) #x

////////////////////////////////////////////////////////////////////////////////

namespace NOB_NAMESPACE {

// ² Encode a version number in a 32 bits integer.
//  Only the major and minor versions are representable on 16 bits each.
//  All zero is a reserved value (#NOB_NOT_AVAILABLE).
#define NOB_VERSION_NUMBER(major, minor)                                       \
    ((((major) % (1 << 16)) << 16) + (((minor) % (1 << 16))))

// Indicate that a feature is not available
#define NOB_NOT_AVAILABLE 0

// Indicate that a feature is available
#define NOB_AVAILABLE (!NOB_NOT_AVAILABLE)

// Based on https://sourceforge.net/p/predef/wiki/Home/

// TODO define all NOB_OS_x and NOB_COMPILER_x constants to 0 and then undef
// and redefine to 1 when appropriate and ifndef DOXYGEN
// Or add an input filter based on
// https://stackoverflow.com/questions/48064256/document-define-without-the-preprocessor

// Detect the current operating system in use.

#if defined(_WIN64)
#define NOB_OS_WINDOWS
#define NOB_OS_WINDOWS_64

#elif defined(_WIN32)
#define NOB_OS_WINDOWS
#define NOB_OS_WINDOWS_32

#elif defined(__APPLE__) || defined(__MACH__)
#define NOB_OS_MACOS

#elif defined(__FreeBSD__)
#define NOB_OS_FREEBSD

#elif defined(__ANDROID__)
#define NOB_OS_ANDROID
#define NOB_OS_LINUX

#elif defined(__linux__)
#define NOB_OS_LINUX

#else
#define NOB_OS_UNSUPPORTED
#endif

// Detect the current compiler in use.

#if defined(__clang__)
#define NOB_COMPILER_CLANG

#elif defined(__MINGW32__)
#define NOB_COMPILER_MINGW
#define NOB_COMPILER_MINGW32

#elif defined(__MINGW64__)
#define NOB_COMPILER_MINGW
#define NOB_COMPILER_MINGW64

#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define NOB_COMPILER_CYGWIN

#elif defined(__GNUC__) || defined(__GNUG__)
#define NOB_COMPILER_GNU

#elif defined(_MSC_VER)
#define NOB_COMPILER_MSVC

#else
#define NOB_COMPILER_UNSUPPORTED
#endif

// Current C++ version number in the format YYYYMM.
// It can be safely compared to the NOB_LANG_CXX_* constants.
#ifdef NOB_COMPILER_MSVC
// MSVC notoriously don't set __cplusplus unless the /Zc:__cplusplus flag is
// used
#define NOB_LANG_CXX _MSVC_LANG
#else
#define NOB_LANG_CXX __cplusplus
#endif

// Known C++ version numbers in the format YYYYMM (same as __cplusplus).
#define NOB_LANG_CXX_98 199711L
#define NOB_LANG_CXX_11 201103L
#define NOB_LANG_CXX_14 201402L
#define NOB_LANG_CXX_17 201703L
#define NOB_LANG_CXX_20 202002L
#define NOB_LANG_CXX_23 202302L

// Accept only environments providing at least C++17.
#if NOB_LANG_CXX < NOB_LANG_CXX_17
#if NOB_COMPILER_MSVC
#error "Use /std:c++17 or greater."
#else // assume GNU-compatible cli
#error "Use -std=c++17 or greater."
#endif
#endif

// Default compiler to use for self-rebuild and when using cmd_append_compiler.
#ifndef NOB_CXX
#if defined(NOB_COMPILER_MSVC)
#define NOB_CXX "cl.exe"
#elif defined(NOB_COMPILER_CLANG)
#define NOB_CXX "clang++"
#else // assume Gcc
#define NOB_CXX "g++"
#endif
#endif

// Default flags to use for self-rebuild and when using
// cmd_append_default_flags.
#ifndef NOB_CXX_DEFAULT_FLAGS
#if NOB_COMPILER_MSVC
#define NOB_CXX_DEFAULT_FLAGS "/W4", "/nologo", "/D_CRT_SECURE_NO_WARNINGS"
#else // assume GNU-compatible cli
#define NOB_CXX_DEFAULT_FLAGS "-Wall", "-Wextra"
#endif
#endif

#ifndef NOB_CXX_OUTPUT_FLAG
#if NOB_COMPILER_MSVC
#define NOB_CXX_OUTPUT_FLAG "/Fe:"
#else
#define NOB_CXX_OUTPUT_FLAG "-o"
#endif
#endif

} // namespace NOB_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

namespace NOB_NAMESPACE {

// Backport of some features of the C++ standard library to C++17.
namespace cxx17 {

// Default source location information.
// Those values are used for compilers that don't provide builtins to get
// source location.
#define NOB_SOURCE_LOCATION_LINE() 0
#define NOB_SOURCE_LOCATION_COLUMN() 0
#define NOB_SOURCE_LOCATION_FILE() "<unknown>"
#define NOB_SOURCE_LOCATION_FUNCTION() "<unknown>"

// Source location supported features.
#define NOB_SOURCE_LOCATION_SUPPORTS_LINE 0
#define NOB_SOURCE_LOCATION_SUPPORTS_COLUMN 0
#define NOB_SOURCE_LOCATION_SUPPORTS_FILE 0
#define NOB_SOURCE_LOCATION_SUPPORTS_FUNCTION 0

#ifdef __has_builtin
// For compilers having __has_builtin, this can be checked at compile-time
// Line info support
#if __has_builtin(__builtin_LINE)
#undef NOB_SOURCE_LOCATION_LINE
#define NOB_SOURCE_LOCATION_LINE() __builtin_LINE()
#undef NOB_SOURCE_LOCATION_SUPPORTS_LINE
#define NOB_SOURCE_LOCATION_SUPPORTS_LINE 1
#endif
// Column info support
#if __has_builtin(__builtin_COLUMN)
#undef NOB_SOURCE_LOCATION_COLUMN
#define NOB_SOURCE_LOCATION_COLUMN() __builtin_COLUMN()
#undef NOB_SOURCE_LOCATION_SUPPORTS_COLUMN
#define NOB_SOURCE_LOCATION_SUPPORTS_COLUMN 1
#endif
// File info support
#if __has_builtin(__builtin_FILE)
#undef NOB_SOURCE_LOCATION_FILE
#define NOB_SOURCE_LOCATION_FILE() __builtin_FILE()
#undef NOB_SOURCE_LOCATION_SUPPORTS_FILE
#define NOB_SOURCE_LOCATION_SUPPORTS_FILE 1
#endif
// Function info support
#if __has_builtin(__builtin_FUNCTION)
#undef NOB_SOURCE_LOCATION_FUNCTION
#define NOB_SOURCE_LOCATION_FUNCTION() __builtin_FUNCTION()
#undef NOB_SOURCE_LOCATION_SUPPORTS_FUNCTION
#define NOB_SOURCE_LOCATION_SUPPORTS_FUNCTION 1
#endif

#elif defined(_MSC_VER) && _MSC_VER >= 1926
// MSVC 2019 version 16.6+ provides those builtins
// Line info support
#undef NOB_SOURCE_LOCATION_LINE
#define NOB_SOURCE_LOCATION_LINE() __builtin_LINE()
#undef NOB_SOURCE_LOCATION_SUPPORTS_LINE
#define NOB_SOURCE_LOCATION_SUPPORTS_LINE 1
// Column info support
#undef NOB_SOURCE_LOCATION_COLUMN
#define NOB_SOURCE_LOCATION_COLUMN() __builtin_COLUMN()
#undef NOB_SOURCE_LOCATION_SUPPORTS_COLUMN
#define NOB_SOURCE_LOCATION_SUPPORTS_COLUMN 1
// File info support
#undef NOB_SOURCE_LOCATION_FILE
#define NOB_SOURCE_LOCATION_FILE() __builtin_FILE()
#undef NOB_SOURCE_LOCATION_SUPPORTS_FILE
#define NOB_SOURCE_LOCATION_SUPPORTS_FILE 1
// Function info support
#undef NOB_SOURCE_LOCATION_FUNCTION
#define NOB_SOURCE_LOCATION_FUNCTION() __builtin_FUNCTION()
#undef NOB_SOURCE_LOCATION_SUPPORTS_FUNCTION
#define NOB_SOURCE_LOCATION_SUPPORTS_FUNCTION 1
#endif

// std::source_location-like class compatible with C++17.
struct SourceLocation {
    static constexpr SourceLocation current(
        uint_least32_t line = NOB_SOURCE_LOCATION_LINE(),
        uint_least32_t column = NOB_SOURCE_LOCATION_COLUMN(),
        const char* file_name = NOB_SOURCE_LOCATION_FILE(),
        const char* function_name = NOB_SOURCE_LOCATION_FUNCTION()) noexcept
    {
        return SourceLocation { line, column, file_name, function_name };
    }

    [[nodiscard]] constexpr std::uint_least32_t line() const noexcept
    {
        return line_;
    }

    [[nodiscard]] constexpr std::uint_least32_t column() const noexcept
    {
        return column_;
    }

    [[nodiscard]] constexpr const char* file_name() const noexcept
    {
        return file_name_;
    }

    [[nodiscard]] constexpr const char* function_name() const noexcept
    {
        return function_name_;
    }

    uint_least32_t line_;
    uint_least32_t column_;
    const char* file_name_;
    const char* function_name_;
};

// Some static checks to ensure that SourceLocation share the same type traits
// properties as std::source_location
static_assert(std::is_default_constructible<SourceLocation>::value);
static_assert(std::is_copy_constructible<SourceLocation>::value);
static_assert(std::is_copy_assignable<SourceLocation>::value);
static_assert(std::is_nothrow_move_constructible<SourceLocation>::value);
static_assert(std::is_nothrow_move_assignable<SourceLocation>::value);
static_assert(std::is_nothrow_swappable<SourceLocation>::value);
static_assert(std::is_destructible<SourceLocation>::value);

} // namespace cxx17

} // namespace NOB_NAMESPACE

// C++17 compatible source_location class.
// Alias to
// [std::source_location](https://en.cppreference.com/w/cpp/utility/source_location.html)
// when building with C++20 onward, otherwise alias to the custom
// cxx17::SourceLocation.
#if NOB_LANG_CXX >= NOB_LANG_CXX_20

#include <source_location>
namespace NOB_NAMESPACE {
using SourceLocation = std::source_location;
// Assume everything is supported
#undef NOB_SOURCE_LOCATION_SUPPORTS_LINE
#define NOB_SOURCE_LOCATION_SUPPORTS_LINE 1
#undef NOB_SOURCE_LOCATION_SUPPORTS_COLUMN
#define NOB_SOURCE_LOCATION_SUPPORTS_COLUMN 1
#undef NOB_SOURCE_LOCATION_SUPPORTS_FILE
#define NOB_SOURCE_LOCATION_SUPPORTS_FILE 1
#undef NOB_SOURCE_LOCATION_SUPPORTS_FUNCTION
#define NOB_SOURCE_LOCATION_SUPPORTS_FUNCTION 1
} // namespace NOB_NAMESPACE

#else

namespace NOB_NAMESPACE {
using SourceLocation = cxx17::SourceLocation;
} // namespace NOB_NAMESPACE

#endif

////////////////////////////////////////////////////////////////////////////////

namespace NOB_NAMESPACE {

// Print a message prefixed with call-site location, then abort.
[[noreturn]]
NOB_DEF void abort_with_message(
    const std::string& message,
    SourceLocation location = SourceLocation::current());

// Print a message prefixed with call-site location and a header, then abort.
[[noreturn]]
NOB_DEF void abort_with_header_and_message(
    const std::string& header,
    const std::string& message,
    SourceLocation location = SourceLocation::current());

// Print a message indicating that something is not yet implemented, then abort.
[[noreturn]]
NOB_DEF void todo(
    const std::string& message,
    SourceLocation location = SourceLocation::current());

// Print a message indicating that an unreachable portion of code was reached,
// then abort.
[[noreturn]]
NOB_DEF void unreachable(
    const std::string& message,
    SourceLocation location = SourceLocation::current());

// Thread-safe implementation of ISO C strerror()
NOB_DEF std::string strerror(int errnum = errno);

} // namespace NOB_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

namespace NOB_NAMESPACE {

// Supported log levels.
enum class LogLevel : uint8_t {
    Debug,
    Info,
    Warning,
    Error,
    Disabled,
};

// Simple yet flexible logger.
// This class is used only for the configuration. Refer to nob::logf to send
// messages.
struct Logger {
    using PrefixBuilder = std::string(LogLevel, void*);

    static Logger& get()
    {
        static Logger logger;
        return logger;
    }

    [[nodiscard]] static std::string default_prefix_builder(
        LogLevel level,
        void* /*userdata*/)
    {
        // TODO add a --color=auto(default)|always|never
        // auto detects if stream is a tty and enable color print
        switch (level) {
        case LogLevel::Debug:
            return "[\e[34mDEBUG  \e[0m] ";
        case LogLevel::Info:
            return "[\e[32mINFO   \e[0m] ";
        case LogLevel::Warning:
            return "[\e[33mWARNING\e[0m] ";
        case LogLevel::Error:
            return "[\e[31mERROR  \e[0m] ";
        default:
            unreachable("nob::Logger::default_prefix_builder");
        }
    };

    LogLevel minimal_log_level = LogLevel::Info;
    FILE* stream = stderr;
    PrefixBuilder* prefix_builder = default_prefix_builder;
    void* userdata = nullptr;
    bool show_source_location = false;
};

struct Fmt {
    const char* str;
    SourceLocation source_location;

    // NOLINTNEXTLINE(*explicit*): intentionally allow implicit conversions
    Fmt(const char* s, const SourceLocation& l = SourceLocation::current())
        : str { s }
        , source_location { l }
    {
    }
};

// Log a message with a specified log level and formatted using a printf-style
// formatting string.
NOB_DEF void logf(LogLevel level, Fmt fmt, ...);

NOB_DEF void logf_debug(Fmt fmt, ...);
NOB_DEF void logf_info(Fmt fmt, ...);
NOB_DEF void logf_warning(Fmt fmt, ...);
NOB_DEF void logf_error(Fmt fmt, ...);

} // namespace NOB_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

// String manipulation utilities.
// Extend the standard library with various operations to manipulate string-like
// objects.

namespace NOB_NAMESPACE {

struct StrJoinOptions {
    // Skip empty strings to avoid having a sequence of delimiter.
    bool skip_empty = false;
};

// Join a range of string-like objects using a delimiter.
template<typename StrRange>
inline void str_join_range(
    std::string& dest,
    const std::string& delimiter,
    const StrRange& pieces,
    StrJoinOptions options = {});

// Join multiple string-like objects using a delimiter.
template<typename... Strs>
inline void str_join(
    std::string& dest,
    const std::string& delimiter,
    Strs&&... strs);

// Printf-format into a std::string.
NOB_DEF std::string str_format(const char* fmt, ...);

} // namespace NOB_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

// Filesystem manipulation utilities.
// Extend the standard library with various operations to manipulate files and
// paths.

namespace NOB_NAMESPACE {

NOB_DEF bool write_entire_file(const char* path, const void* data, size_t size);

} // namespace NOB_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

namespace NOB_NAMESPACE {

using Proc = int;
constexpr Proc NOB_INVALID_PROC = -1;

NOB_DEF bool proc_wait(Proc proc);

} // namespace NOB_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

namespace NOB_NAMESPACE {

using Inputs = std::vector<std::string>;
using Outputs = std::vector<std::string>;
using CommandLine = std::vector<std::string>;

// A command to process inputs into outputs using a command line.
// Keeps track of the inputs and outputs separately from the command line t
// ease the generation of a compilation database.
struct Cmd {
    std::string name;
    Inputs inputs;
    Outputs outputs;
    CommandLine command_line;
};

// A group of related commands to run.
using CmdGroup = std::vector<Cmd>;

NOB_DEF void cmd_append(Cmd& cmd, const std::string& arg);

NOB_DEF void cmd_append_many(Cmd& cmd, const std::vector<std::string>& args);

NOB_DEF void cmd_append_cxx_compiler(Cmd& cmd);

NOB_DEF void cmd_append_cxx_default_flags(Cmd& cmd);

NOB_DEF void cmd_append_input(Cmd& cmd, const std::string& input);

NOB_DEF void cmd_append_inputs(
    Cmd& cmd,
    const std::vector<std::string>& inputs);

NOB_DEF void cmd_append_output(Cmd& cmd, const std::string& output);

NOB_DEF void cmd_append_outputs(
    Cmd& cmd,
    const std::vector<std::string>& outputs);

NOB_DEF std::string cmd_render(const Cmd& cmd);

NOB_DEF Proc cmd_start_process(const Cmd& cmd);

NOB_DEF bool cmd_run(const Cmd& cmd);

NOB_DEF std::string into_object_file(
    std::string_view input_file,
    std::string_view build_dir);

// Generate a compilation database from given command groups into a file
// See https://clang.llvm.org/docs/JSONCompilationDatabase.html
#define NOB_COMPDB_FILE "compile_commands.json"
NOB_DEF bool compdb_dump(
    const std::vector<CmdGroup>& cmd_groups,
    const char* file);

} // namespace NOB_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

// Go Rebuild Yourself™ Technology
//
// By using the NOB_GO_REBUILD_YOURSELF* macros, nob will automatically attempt
// to rebuild itself upon execution if at least one of the input files was
// changed on disk

namespace NOB_NAMESPACE {

// Shift an array by 1 element, decreasing the element count, and return it.
// Similar to bash shift, it is especially useful to handle program arguments.
template<typename T, typename SizeType = size_t>
T& shift(SizeType& count, T*& values)
{
    assert(count > 0);
    count--;
    return *values++; // NOLINT(*pointer-arithmetic*)
}

struct RebuildInfo {
    RebuildInfo& add_source_file(const std::string& file)
    {
        source_files.push_back(file);
        return *this;
    }
    RebuildInfo& add_source_files(const std::vector<std::string>& files)
    {
        source_files.insert(files.end(), files.begin(), files.end());
        return *this;
    }
    RebuildInfo& add_header_file(const std::string& file)
    {
        header_files.push_back(file);
        return *this;
    }
    RebuildInfo& add_header_files(const std::vector<std::string>& files)
    {
        header_files.insert(files.end(), files.begin(), files.end());
        return *this;
    }
    RebuildInfo& add_option(const std::string& opt)
    {
        options.push_back(opt);
        return *this;
    }
    RebuildInfo& add_options(const std::vector<std::string>& opts)
    {
        options.insert(options.end(), opts.begin(), opts.end());
        return *this;
    }
    std::vector<std::string> source_files;
    std::vector<std::string> header_files;
    std::vector<std::string> options;
};

namespace rebootstrap {

// Rebuild nob if needed and then re-execute the binary.
#define NOB_GO_REBUILD_YOURSELF(argc, argv)                                    \
    nob::rebootstrap::go_rebuild_yourself((argc), (argv), __FILE__)

// Rebuild nob if needed with additional build options and then re-execute the
// binary.
#define NOB_GO_REBUILD_YOURSELF_WITH_INFO(argc, argv, rebuild_info)            \
    nob::rebootstrap::go_rebuild_yourself(                                     \
        (argc),                                                                \
        (argv),                                                                \
        __FILE__,                                                              \
        (rebuild_info))

// Rebootstraper implementation details

#ifndef NOB_REBOOTSTRAP_COMPILER
#define NOB_REBOOTSTRAP_COMPILER NOB_CXX
#endif

#ifdef NOB_COMPILER_MSVC
#define NOB_REBOOTSTRAP_FLAG_STDCXX "/std:c++"
#else // Assume GNU-compatible cli
#define NOB_REBOOTSTRAP_FLAG_STDCXX "-std=c++"
#endif

// TODO handle cases where user bootstrapped nob with a more recent standard
constexpr const char flag_required_cxx_version[]
    = NOB_REBOOTSTRAP_FLAG_STDCXX "17";

enum [[nodiscard]] NeedsRebuildResult {
    No,
    Yes,
    Error,
};

NOB_DEF NeedsRebuildResult needs_rebuild(
    const std::string& output_file,
    const std::vector<std::string>& input_files);

NOB_DEF void go_rebuild_yourself(
    int argc,
    char** argv,
    const char* main_source_file,
    RebuildInfo rebuild_info = {});

} // namespace rebootstrap

} // namespace NOB_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

// Inline nob.hpp implementation.
// All functions defined here MUST NOT be declared NOB_DEF, and MUST be declared
// inline at the declaration site.

namespace NOB_NAMESPACE {

template<typename StrRange>
void str_join_range(
    std::string& dest,
    const std::string& delimiter,
    const StrRange& pieces,
    StrJoinOptions options)
{
    // Worst case allocation
    size_t to_append = dest.size();
    to_append += delimiter.size() * pieces.size();
    for (const auto& piece : pieces) {
        to_append += piece.size();
    }
    dest.reserve(to_append);

    // TODO avoid adding the delimiter before the first piece
    for (const auto& piece : pieces) {
        if (options.skip_empty && piece.empty()) {
            continue;
        }
        dest += delimiter;
        dest += piece;
    }
}

template<typename... Strs>
void str_join(std::string& dest, const std::string& delimiter, Strs&&... strs)
{
    str_join_range(
        dest,
        delimiter,
        { (..., std::string_view(std::forward<Strs>(strs))) });
}

} // namespace NOB_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

// Out-of-line nob.hpp implementation.
// All functions defined here MUST be declared NOB_DEF.

// #ifdef NOB_IMPLEMENTATION

#include <cstdarg>
#include <filesystem>
#include <system_error>

#ifdef NOB_OS_WINDOWS
#error Windows not supported yet
#else
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace NOB_NAMESPACE {

[[noreturn]]
NOB_DEF void abort_with_message(
    const std::string& message,
    SourceLocation location)
{
    // Force the usage of stderr as this is an internal error
    (void)fprintf(
        stderr,
#if NOB_SOURCE_LOCATION_SUPPORTS_FILE
        "%s:"
#if NOB_SOURCE_LOCATION_SUPPORTS_LINE
        "%u:"
#endif
        " "
#endif
        "%s\n",
#if NOB_SOURCE_LOCATION_SUPPORTS_FILE
        location.file_name(),
#endif
#if NOB_SOURCE_LOCATION_SUPPORTS_LINE
        location.line(),
#endif
        message.c_str());
    std::abort();
}

[[noreturn]]
NOB_DEF void abort_with_header_and_message(
    const std::string& header,
    const std::string& message,
    SourceLocation location)
{
    std::string msg;
    msg.reserve(header.size() + 2 + message.size());
    msg += header;
    msg += ": ";
    msg += message;
    abort_with_message(msg, location);
}

[[noreturn]]
NOB_DEF void todo(const std::string& message, SourceLocation location)
{
    abort_with_header_and_message("Todo", message, location);
}

[[noreturn]]
NOB_DEF void unreachable(const std::string& message, SourceLocation location)
{
    abort_with_header_and_message("Unreachable", message, location);
}

NOB_DEF std::string strerror(int errnum)
{
    return std::error_code(errnum, std::generic_category()).message();
}

static void vlogf(LogLevel level, Fmt fmt, va_list args)
{
    auto& logger = Logger::get();
    if (level < logger.minimal_log_level || level == LogLevel::Disabled) {
        return;
    }

    if (logger.prefix_builder != nullptr) {
        auto prefix = logger.prefix_builder(level, logger.userdata);
        (void)fputs(prefix.c_str(), logger.stream);
    }
    (void)vfprintf(logger.stream, fmt.str, args);
    (void)fputs("\n", logger.stream);
    if (logger.show_source_location) {
#if NOB_SOURCE_LOCATION_SUPPORTS_FILE && NOB_SOURCE_LOCATION_SUPPORTS_LINE
        (void)fprintf(
            logger.stream,
            "\e[2m[from: %s:%u]\e[0m\n",
            fmt.source_location.file_name(),
            fmt.source_location.line());
#else
        // Temporarily disable source location tracing to avoid infinite
        // recursion
        logger.show_source_location = false;
        logf_warning("Source location trace requested but not supported");
        logger.show_source_location = true;
#endif
    }
}

NOB_DEF void logf(LogLevel level, Fmt fmt, ...)
{
    auto& logger = Logger::get();
    if (level < logger.minimal_log_level || level == LogLevel::Disabled) {
        return;
    }

    if (logger.prefix_builder != nullptr) {
        auto prefix = logger.prefix_builder(level, logger.userdata);
        (void)fputs(prefix.c_str(), logger.stream);
    }
    va_list args;
    va_start(args, fmt);
    vlogf(level, fmt, args);
    va_end(args);
}

NOB_DEF void logf_debug(Fmt fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlogf(LogLevel::Debug, fmt, args);
    va_end(args);
}

NOB_DEF void logf_info(Fmt fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlogf(LogLevel::Info, fmt, args);
    va_end(args);
}

NOB_DEF void logf_warning(Fmt fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlogf(LogLevel::Warning, fmt, args);
    va_end(args);
}

NOB_DEF void logf_error(Fmt fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlogf(LogLevel::Error, fmt, args);
    va_end(args);
}

NOB_DEF std::string str_printf(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    std::string str;
    str.resize(n);
    va_start(args, fmt);
    vsnprintf(str.data(), n + 1, fmt, args);
    va_end(args);

    return str;
}

NOB_DEF bool write_entire_file(const char* path, const void* data, size_t size)
{
    bool result = true;

    const char* buf = NULL;
    FILE* f = fopen(path, "wb");
    if (f == NULL) {
        logf_error(
            "Could not open file `%s` for writing: %s\n",
            path,
            strerror(errno).c_str());
        result = false;
        goto defer;
    }

    //           len
    //           v
    // aaaaaaaaaa
    //     ^
    //     data

    buf = (const char*)data;
    while (size > 0) {
        size_t n = fwrite(buf, 1, size, f);
        if (ferror(f)) {
            logf_error(
                "Could not write into file `%s`: %s\n",
                path,
                strerror(errno).c_str());
            result = false;
            goto defer;
        }
        size -= n;
        buf += n;
    }

defer:
    if (f) {
        fclose(f);
    }
    return result;
}

NOB_DEF bool proc_wait(Proc proc)
{
    if (proc == NOB_INVALID_PROC) {
        logf_error("Invalid proc object");
        return false;
    }

    for (;;) {
        int wait_status = 0;
        if (waitpid(proc, &wait_status, 0) < 0) {
            logf_error(
                "Could not wait on command (pid %d): %s",
                proc,
                strerror().c_str());
            return false;
        }
        if (WIFEXITED(wait_status)) {
            int exit_status = WEXITSTATUS(wait_status);
            if (exit_status != 0) {
                logf_error("Command exited with exit code %d", exit_status);
                return false;
            }
            break;
        }
        if (WIFSIGNALED(wait_status)) {
            logf_error(
                "Command process was terminated by signal %d",
                WTERMSIG(wait_status));
            return false;
        }
    }

    return true;
}

NOB_DEF void cmd_append(Cmd& cmd, const std::string& arg)
{
    cmd.command_line.push_back(arg);
}

NOB_DEF void cmd_append_many(Cmd& cmd, const std::vector<std::string>& args)
{
    for (const auto& arg : args) {
        cmd_append(cmd, arg);
    }
}

NOB_DEF void cmd_append_cxx_compiler(Cmd& cmd)
{
    cmd_append(cmd, NOB_CXX);
}

NOB_DEF void cmd_append_cxx_default_flags(Cmd& cmd)
{
    cmd_append_many(cmd, { NOB_CXX_DEFAULT_FLAGS });
}

NOB_DEF void cmd_append_input(Cmd& cmd, const std::string& input)
{
    cmd.inputs.push_back(input);
    cmd.command_line.push_back(input);
}

NOB_DEF void cmd_append_inputs(Cmd& cmd, const std::vector<std::string>& inputs)
{
    for (const auto& input : inputs) {
        cmd_append_input(cmd, input);
    }
}

NOB_DEF void cmd_append_output(Cmd& cmd, const std::string& output)
{
    cmd.outputs.push_back(output);
    cmd.command_line.push_back(NOB_CXX_OUTPUT_FLAG);
    cmd.command_line.push_back(output);
}

NOB_DEF void cmd_append_outputs(
    Cmd& cmd,
    const std::vector<std::string>& outputs)
{
    for (const auto& output : outputs) {
        cmd_append_output(cmd, output);
    }
}

NOB_DEF std::string cmd_render(const CommandLine& cmd)
{
    std::string str;
    StrJoinOptions options;
    options.skip_empty = true;
    str_join_range(str, " ", cmd, options);
    return str;
}

NOB_DEF Proc cmd_start_process(const CommandLine& cmdline)
{
    if (cmdline.size() < 1) {
        logf_error("Could not run empty command");
        return NOB_INVALID_PROC;
    }

    logf_debug("CMD: %s", cmd_render(cmdline).c_str());

    pid_t cpid = fork();
    if (cpid < 0) {
        logf_error("Could not fork child process: %s", strerror().c_str());
        return NOB_INVALID_PROC;
    }

    if (cpid == 0) {
        // Construct the null-terminated argv array from a local `cmd`
        // copy as the child is allowed to modify it
        CommandLine args = cmdline;
        std::vector<char*> cmd_null;
        cmd_null.reserve(args.size() + 1);
        for (auto& token : args) {
            // NOLINTNEXTLINE(*data-pointer*): std::string::data() returns non-const data only since C++17
            cmd_null.push_back(&token[0]);
        }
        cmd_null.push_back(nullptr);

        if (execvp(cmd_null[0], cmd_null.data()) < 0) {
            logf_error(
                "Could not exec child process for `%s`: %s",
                cmdline[0].c_str(),
                strerror().c_str());
            _exit(1);
        }
    }

    return cpid;
}

NOB_DEF bool cmd_run(const CommandLine& command_line)
{
    Proc proc = cmd_start_process(command_line);
    return proc_wait(proc);
}

NOB_DEF std::string into_object_file(
    std::string_view input_file,
    std::string_view build_dir)
{
    std::string output;
    output += build_dir;
    output += "/";
    output += input_file;
    output += ".o";
    return output;
}

using CompDb = std::string;

static void compdb_start(CompDb& compdb)
{
    compdb += "[";
}

static void compdb_add_entry(CompDb& compdb, const Cmd& cmd)
{
    if (cmd.inputs.size() != 1 || cmd.outputs.size() != 1) {
        logf_warning(
            "The Cmd objects used for compdb generation must have "
            "exactly one input and one output file, ignoring current command.");
        return;
    }
    auto cwd = std::filesystem::current_path();
    std::string command_str = cmd_render(cmd.command_line);
    compdb += str_printf(
        "\n"
        "  {\n"
        "    \"directory\": \"%s\",\n"
        "    \"command\": \"%s\",\n"
        "    \"file\": \"%s\",\n"
        "    \"output\": \"%s\"\n"
        "  }",
        cwd.c_str(),
        command_str.c_str(),
        cmd.inputs[0].c_str(),
        cmd.outputs[0].c_str());
}

static void compdb_entry_continue(CompDb& compdb)
{
    compdb += ",";
}

static void compdb_end(CompDb& compdb)
{
    compdb += "\n]\n";
}

NOB_DEF bool compdb_dump(
    const std::vector<CmdGroup>& cmd_groups,
    const char* file)
{
    bool result = true;
    CompDb compdb;
    // 1MiB compile_commands.json should be enough to start with
    compdb.reserve(1024ULL * 1024);
    {
        compdb_start(compdb);
        {
            // const size_t cmd_group_count = cmd_goups.size();
            // for (auto& cmd_group : cmd_goups) {
            //     const size_t cmds_count = cmd_group.size();
            //     for (auto& cmd : cmd_group) {
            //         compdb_add_entry(&compdb, cmd);
            //         if (group < cmd_group_count - 1
            //             || i < cmd_group->count - 1) {
            //             compdb_entry_continue(compdb);
            //         }
            //     }
            // }
        }
        compdb_end(compdb);
    }
    result = write_entire_file(file, compdb.data(), compdb.length());
    return result;
}

namespace rebootstrap {

NOB_DEF NeedsRebuildResult needs_rebuild(
    const std::string& output_files,
    const std::vector<std::string>& input_files)
{
    struct stat statbuf { };
    if (stat(output_files.c_str(), &statbuf) < 0) {
        if (errno == ENOENT) {
            return NeedsRebuildResult::Yes;
        }
        logf_error(
            "Could not stat `%s`: %s",
            output_files.c_str(),
            strerror().c_str());
        return NeedsRebuildResult::Error;
    }
    auto output_file_time = statbuf.st_mtime;

    for (const auto& input_file : input_files) {
        struct stat statbuf { };
        if (stat(input_file.c_str(), &statbuf) < 0) {
            // Non-existing input is an error because it is needed for
            // building in the first place
            logf_error(
                "Could not stat `%s`: %s",
                input_file.c_str(),
                strerror().c_str());
            return NeedsRebuildResult::Error;
        }
        auto input_file_time = statbuf.st_mtime;
        if (input_file_time > output_file_time) {
            return NeedsRebuildResult::Yes;
        }
    }

    return NeedsRebuildResult::No;
}

NOB_DEF void go_rebuild_yourself(
    int argc,
    char** argv,
    const char* main_source_file,
    RebuildInfo rebuild_info)
{
    // Add caller source file
    rebuild_info.add_source_file(main_source_file);
    // Add nob.hpp header file
    rebuild_info.add_header_file(__FILE__);

    const std::string binary_path = shift(argc, argv);
    const std::string old_binary_path = binary_path + ".old";

    // Rebuild only if input file have changed
    std::vector<std::string> input_files;
    input_files.insert(
        input_files.end(),
        rebuild_info.source_files.begin(),
        rebuild_info.source_files.end());
    input_files.insert(
        input_files.end(),
        rebuild_info.header_files.begin(),
        rebuild_info.header_files.end());
    auto result = needs_rebuild(binary_path, input_files);
    if (result == NeedsRebuildResult::Error) {
        std::exit(1); // NOLINT(concurrency-mt-unsafe)
    }
    if (result == NeedsRebuildResult::No) {
        return;
    }

    logf_info(
        "Nob input files out-of-date, regenerating %s...",
        binary_path.c_str());

    if (std::rename(binary_path.c_str(), old_binary_path.c_str()) != 0) {
        logf_error("Could not rename binary into old: %s", strerror().c_str());
        std::exit(1); // NOLINT(concurrency-mt-unsafe)
    }
    // TODO add support for CXX, CXXFLAGS, LDFLAGS
    CommandLine rebuild_cmd {
        NOB_REBOOTSTRAP_COMPILER,
        NOB_CXX_OUTPUT_FLAG,
        binary_path,
        flag_required_cxx_version,
    };
    rebuild_cmd.insert(
        rebuild_cmd.end(),
        rebuild_info.options.begin(),
        rebuild_info.options.end());
    rebuild_cmd.insert(
        rebuild_cmd.end(),
        rebuild_info.source_files.begin(),
        rebuild_info.source_files.end());

    if (!cmd_run(rebuild_cmd)) {
        logf_warning("Rebuild failed, restoring old binary");
        if (std::rename(old_binary_path.c_str(), binary_path.c_str()) != 0) {
            logf_error("Could not restore old binary: %s", strerror().c_str());
        }
        std::exit(1); // NOLINT(concurrency-mt-unsafe)
    }

    if (std::remove(old_binary_path.c_str()) != 0) {
        logf_warning("Could not delete old binary: %s", strerror().c_str());
    }

    CommandLine reexec_cmd { binary_path };
    // NOLINTNEXTLINE(*pointer-arithmetic*)
    reexec_cmd.insert(reexec_cmd.end(), argv, argv + argc);

    // TODO add timing information
    logf_info("Generating done");
    if (!cmd_run(reexec_cmd)) {
        std::exit(1); // NOLINT(concurrency-mt-unsafe)
    }

    std::exit(0); // NOLINT(concurrency-mt-unsafe)
}

} // namespace rebootstrap

} // namespace NOB_NAMESPACE

// #endif // NOB_IMPLEMENTATION

#endif // NOB_HPP_
