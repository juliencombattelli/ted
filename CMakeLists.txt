cmake_minimum_required(VERSION 3.20)
project(ted)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_REQUIRED yes)
set(CMAKE_CXX_EXTENSIONS no)

find_package(ICU REQUIRED COMPONENTS uc)

if(WIN32)
    set(PLATFORM_DIR win32)
elseif(UNIX)
    set(PLATFORM_DIR posix)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

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
target_include_directories(libted PUBLIC src)
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
target_link_libraries(utf8_test PRIVATE libted)

add_executable(kilo examples/kilo.c)
