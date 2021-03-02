set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

if (pclient_WANT_TESTS)
    add_subdirectory(googletest)
endif ()

add_subdirectory(cxxopts)
