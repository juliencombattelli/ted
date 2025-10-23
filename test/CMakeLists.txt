
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         v1.17.0
    GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(googletest)

add_executable(utf8_test
    utf8_test.cpp
)
target_link_libraries(utf8_test PRIVATE libted GTest::gtest)

add_executable(strong_type_test
    strong_type_test.cpp
)
target_link_libraries(strong_type_test PRIVATE libted GTest::gtest_main)
