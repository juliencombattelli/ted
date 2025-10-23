cmake_minimum_required(VERSION 3.25)
project(TED VERSION 0.1.0)

option(TED_BUILD_TESTS "Enable building the tests" ON)
option(TED_BUILD_KILO "Enable building the Kilo reference editor" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ICU REQUIRED COMPONENTS uc)

if(WIN32)
    set(PLATFORM_DIR win32)
elseif(UNIX)
    set(PLATFORM_DIR posix)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

configure_file(src/ted/config.hpp.in include/ted/config.hpp @ONLY)

include(cmake/FeatureTesting.cmake)

macro(ted_required_features)
    # Some GCC versions support C++23 but not deducing this...
    have_cxx_feature(__cpp_explicit_this_parameter 202110L MISSING_FEATURES)
endmacro()

check_features(ted_required_features)

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

set(TED_WARNINGS_GNU_COMPATIBLE
    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wcast-qual
    -Wdouble-promotion -Wfloat-equal -Wformat=2 -Wformat-signedness
    -Winit-self -Wmissing-declarations -Wswitch-default -Wswitch-enum
    -Wundef -Wunused-macros -Wwrite-strings
    # -Wpadded
)
set(TED_WARNINGS_GNU_EXCLUSIVE
    -Wduplicated-branches -Wduplicated-cond -Wlogical-op
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(libted PUBLIC ${TED_WARNINGS_GNU_COMPATIBLE})
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(libted PUBLIC ${TED_WARNINGS_GNU_EXCLUSIVE})
    endif()
endif()

add_executable(ted
    src/main.cpp
)
target_link_libraries(ted PRIVATE libted)

if(TED_BUILD_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
endif()

if(TED_BUILD_KILO)
    add_executable(kilo examples/kilo.c)
endif()
