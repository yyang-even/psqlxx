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
    db.cpp
    db.hpp
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

add_gtest_for(args)
add_gtest_for(db)

if (psqlxx_WANT_INSTALLER)
    install(
        TARGETS psqlxx_main
        EXPORT psqlxx_apps
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT psqlxx_apps)
endif ()
