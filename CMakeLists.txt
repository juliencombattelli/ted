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

add_executable(ted
    src/main.cpp
    src/ted/editor.cpp
    src/ted/os.cpp
    src/ted/term.cpp
    src/ted/tui.cpp
    src/ted/utf8.cpp
    src/ted/platform/${PLATFORM_DIR}/os.cpp
    src/ted/platform/${PLATFORM_DIR}/term.cpp
)
target_include_directories(ted PRIVATE src)
target_link_libraries(ted PRIVATE ICU::uc)

add_executable(kilo examples/kilo.c)
