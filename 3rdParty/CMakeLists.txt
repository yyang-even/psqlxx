set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

if (psqlxx_WANT_TESTS)
    add_subdirectory(googletest)
endif ()

option(CXXOPTS_BUILD_TESTS "Set to ON to build cxxopts tests" OFF)
add_subdirectory(cxxopts)

option(BUILD_TEST "Set to ON to build libpqxx tests" OFF)
add_subdirectory(libpqxx)
