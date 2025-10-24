cmake_minimum_required(VERSION 3.25)
project(TED VERSION 0.1.0)

option(TED_BUILD_TESTS "Enable building the tests" ON)
option(TED_BUILD_KILO "Enable building the Kilo reference editor" ON)

# Set a default build type if none was specified
set(TED_DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(TED_DEFAULT_BUILD_TYPE "RelWithDebInfo")
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${TED_DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${TED_DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

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

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
    message(FATAL_ERROR "GCC>=14 is required")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19)
    message(FATAL_ERROR "Clang>=19 is required")
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
target_include_directories(libted PUBLIC src "${CMAKE_CURRENT_BINARY_DIR}/include")
target_link_libraries(libted PRIVATE ICU::uc)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(SANITIZERS address,leak,undefined,pointer-compare,pointer-subtract)
    target_compile_options(libted PUBLIC
        # Basic warning set
        -Wall -Wextra -Wpedantic
        # Additional misc warnings
        -Wshadow -Winit-self -Wmissing-declarations -Wdate-time
        # Cast related warnings
        -Wcast-qual -Wold-style-cast
        $<$<CXX_COMPILER_ID:GNU>:-Wuseless-cast>
        # Conversion related warnings
        -Wconversion -Wsign-conversion -Wfloat-conversion -Wenum-conversion
        $<$<CXX_COMPILER_ID:GNU>:-Warith-conversion>
        # Floating points related warnings
        -Wdouble-promotion -Wfloat-equal
        # Format strings related warnings
        -Wformat=2 -Wformat-signedness
        $<$<CXX_COMPILER_ID:GNU>:-Wformat-overflow=2 -Wformat-truncation=2>
        # Switch related warnings
        -Wswitch-default -Wswitch-enum
        -Wimplicit-fallthrough$<$<CXX_COMPILER_ID:GNU>:=5>
        # Preprocessor related warnings
        -Wundef -Wunused-macros
        # Performance degradation warnings
        # -Wpadded
        $<$<CXX_COMPILER_ID:GNU>:
            -Wnrvo -Wduplicated-branches -Wduplicated-cond -Wlogical-op
        >
        # Hardening options
        -fsanitize=${SANITIZERS}
        -fsanitize-address-use-after-scope
        -fno-omit-frame-pointer
        -fstrict-aliasing
        $<$<CXX_COMPILER_ID:GNU>:-fhardened -Wno-hardened>
    )
    target_link_options(libted PUBLIC -fsanitize=${SANITIZERS})
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
