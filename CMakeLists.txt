cmake_minimum_required(VERSION 3.20)
project(ted)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_REQUIRED yes)

if(WIN32)
    set(PLATFORM_DIR win32)
elseif(UNIX)
    set(PLATFORM_DIR posix)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_executable(ted
    src/main.cpp
    src/platform/${PLATFORM_DIR}/term.cpp
)
target_include_directories(ted PRIVATE src)
