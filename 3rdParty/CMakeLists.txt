set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

if (pclient_WANT_TESTS)
    add_subdirectory(googletest)
endif ()

option(CXXOPTS_BUILD_TESTS "Set to ON to build tests" OFF)
add_subdirectory(cxxopts)
