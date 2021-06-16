set(COMPILER_WARNING_OPTIONS -Wall -Wextra -pedantic-errors)
if (psqlxx_WARNINGS_AS_ERRORS)
    list(APPEND COMPILER_WARNING_OPTIONS -Werror)
endif ()

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibEdit REQUIRED IMPORTED_TARGET libedit>=3.1)

configure_file(version.cpp.in version.cpp @ONLY)
add_library(psqlxx_version STATIC ${CMAKE_CURRENT_BINARY_DIR}/version.cpp version.hpp)
add_library(psqlxx::version ALIAS psqlxx_version)
set_target_properties(psqlxx_version PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(psqlxx_version PRIVATE ${COMPILER_WARNING_OPTIONS})

add_library(
    psqlxx_psqlxx
    args.cpp
    args.hpp
    cli.cpp
    cli.hpp
    command.cpp
    command.hpp
    db.cpp
    db.hpp
    exception.hpp
    formatter.cpp
    formatter.hpp
    string_utils.hpp)
add_library(psqlxx::psqlxx ALIAS psqlxx_psqlxx)
target_link_libraries(
    psqlxx_psqlxx
    PRIVATE psqlxx::version PkgConfig::LibEdit
    PUBLIC cxxopts pqxx)
target_compile_options(psqlxx_psqlxx PUBLIC ${COMPILER_WARNING_OPTIONS})

add_executable(psqlxx_main main.cpp)
add_executable(psqlxx::main ALIAS psqlxx_main)
target_link_libraries(psqlxx_main PRIVATE psqlxx::psqlxx)
set_target_properties(psqlxx_main PROPERTIES OUTPUT_NAME psqlxx)

add_test(NAME psqlxx.main.can_display_help COMMAND psqlxx_main --help)
add_test(NAME psqlxx.main.can_display_version COMMAND psqlxx_main --version)
enable_auto_test_command(psqlxx_main ^psqlxx.main)

discover_gtest_for(args psqlxx::psqlxx)
discover_gtest_for(command psqlxx::psqlxx)
discover_gtest_for(db psqlxx::psqlxx)
discover_gtest_for(string_utils)

configure_file(test_utils.cpp.in test_utils.cpp @ONLY)
add_library(psqlxx_test_utils ${CMAKE_CURRENT_BINARY_DIR}/test_utils.cpp test_utils.hpp)
add_library(psqlxx::test_utils ALIAS psqlxx_test_utils)
target_compile_options(psqlxx_test_utils PUBLIC ${COMPILER_WARNING_OPTIONS})

add_gtest_for(real_db psqlxx::psqlxx psqlxx::test_utils)
set_tests_properties(psqlxx.real_db.test PROPERTIES FIXTURES_REQUIRED RealDbTests)

if (psqlxx_WANT_INSTALLER)
    install(
        TARGETS psqlxx_main
        EXPORT psqlxx_apps
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT psqlxx_apps)
endif ()
