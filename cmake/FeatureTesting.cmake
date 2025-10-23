
function(have_cxx_feature FEATURE_TEST_MACRO VERSION MISSING_FEATURE_LIST)
    set(TRY_COMPILE_SOURCE "
        #if ${FEATURE_TEST_MACRO} < ${VERSION}
            #error
        #endif
        int main() {}
    ")
    set(CACHE_VAR HAVE_${FEATURE_TEST_MACRO})
    if(NOT DEFINED ${CACHE_VAR})
        try_compile(${CACHE_VAR}
            SOURCE_FROM_VAR try_compile.cpp TRY_COMPILE_SOURCE
            OUTPUT_VARIABLE TRY_COMPILE_OUTPUT
        )
        if(NOT ${CACHE_VAR})
            message(STATUS "Checking for C++ feature `${FEATURE_TEST_MACRO}` - Not available")
            list(APPEND ${MISSING_FEATURE_LIST} ${FEATURE_TEST_MACRO})
            set(${MISSING_FEATURE_LIST} ${${MISSING_FEATURE_LIST}} PARENT_SCOPE)
        else()
            message(STATUS "Checking for C++ feature `${FEATURE_TEST_MACRO}` - Available")
        endif()
    endif()
endfunction()

function(check_features FEATURE_TESTING_CALLBACK)
    if(NOT DEFINED ALL_FEATURES_FOUND)
        message(CHECK_START "Checking for required C++ features")
        list(APPEND CMAKE_MESSAGE_INDENT "  ")
        unset(MISSING_FEATURES)

        cmake_language(CALL ${FEATURE_TESTING_CALLBACK})

        list(POP_BACK CMAKE_MESSAGE_INDENT)
        if(MISSING_FEATURES)
            message(CHECK_FAIL "Failed")
            message(FATAL_ERROR "Missing features: ${MISSING_FEATURES}")
        else()
            message(CHECK_PASS "Success")
            set(ALL_FEATURES_FOUND TRUE CACHE INTERNAL "")
        endif()
    endif()
endfunction()
