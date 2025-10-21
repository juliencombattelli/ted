cmake_minimum_required(VERSION 3.20)
project(TED VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_REQUIRED yes)
set(CMAKE_CXX_EXTENSIONS no)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         v1.17.0
    GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(googletest)

find_package(ICU REQUIRED COMPONENTS uc)

if(WIN32)
    set(PLATFORM_DIR win32)
elseif(UNIX)
    set(PLATFORM_DIR posix)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

configure_file(src/ted/config.hpp.in include/ted/config.hpp @ONLY)

add_library(libted STATIC
    src/ted/editor.cpp
    src/ted/os.cpp
    src/ted/term.cpp
    src/ted/tui.cpp
    src/ted/utf8.cpp
    src/ted/platform/${PLATFORM_DIR}/os.cpp
    src/ted/platform/${PLATFORM_DIR}/term.cpp
)
set_target_properties(libted PROPERTIES PREFIX "")
target_include_directories(libted PUBLIC src "${CMAKE_CURRENT_BINARY_DIR}/include")
target_link_libraries(libted PRIVATE ICU::uc)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(libted PUBLIC
    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wcast-qual
    -Wdouble-promotion -Wduplicated-branches -Wduplicated-cond -Wfloat-equal
    -Wformat=2 -Wformat-signedness -Winit-self -Wlogical-op
    -Wmissing-declarations -Wswitch-default -Wswitch-enum
    -Wundef -Wunused-macros -Wwrite-strings
    # -Wpadded
)
endif()

add_executable(ted
    src/main.cpp
)
target_link_libraries(ted PRIVATE libted)

add_executable(utf8_test
    src/utf8_test.cpp
)
target_link_libraries(utf8_test PRIVATE libted GTest::gtest)

add_executable(kilo examples/kilo.c)
