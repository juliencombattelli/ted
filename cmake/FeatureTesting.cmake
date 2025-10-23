
function(have_cxx_feature FEATURE_TEST_MACRO VERSION MISSING_FEATURE_LIST)
    set(TRY_COMPILE_SOURCE "
        #if ${FEATURE_TEST_MACRO} < ${VERSION}
            #error
        #endif
        int main() {}
    ")
    set(CACHE_VAR HAVE_${FEATURE_TEST_MACRO})
    if(NOT DEFINED ${CACHE_VAR})
        message(CHECK_START "Checking for C++ feature `${FEATURE_TEST_MACRO}`")
        try_compile(${CACHE_VAR}
            SOURCE_FROM_VAR try_compile.cpp TRY_COMPILE_SOURCE
            OUTPUT_VARIABLE TRY_COMPILE_OUTPUT
        )
        if(NOT ${CACHE_VAR})
            message(CHECK_FAIL "Not available")
            list(APPEND ${MISSING_FEATURE_LIST} ${FEATURE_TEST_MACRO})
            set(${MISSING_FEATURE_LIST} ${${MISSING_FEATURE_LIST}} PARENT_SCOPE)
        else()
            message(CHECK_PASS "Available")
        endif()
    endif()
endfunction()
