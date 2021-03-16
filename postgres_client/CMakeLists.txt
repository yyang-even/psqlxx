set(COMPILER_WARNING_OPTIONS -Wall -Wextra -pedantic-errors)
if (pclient_WARNINGS_AS_ERRORS)
    list(APPEND COMPILER_WARNING_OPTIONS -Werror)
endif ()

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibEdit REQUIRED IMPORTED_TARGET libedit>=3.1)

configure_file(version.cpp.in version.cpp @ONLY)
add_library(pclient_version STATIC ${CMAKE_CURRENT_BINARY_DIR}/version.cpp version.hpp)
add_library(pclient::version ALIAS pclient_version)
set_target_properties(pclient_version PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(pclient_version PRIVATE ${COMPILER_WARNING_OPTIONS})

add_library(pclient_pclient args.cpp args.hpp cli.cpp cli.hpp sql_session.cpp
                            sql_session.hpp)
add_library(pclient::pclient ALIAS pclient_pclient)
target_link_libraries(
    pclient_pclient
    PRIVATE pclient::version PkgConfig::LibEdit
    PUBLIC cxxopts pqxx)
target_compile_options(pclient_pclient PUBLIC ${COMPILER_WARNING_OPTIONS})

add_executable(pclient_main main.cpp)
add_executable(pclient::main ALIAS pclient_main)
target_link_libraries(pclient_main PRIVATE pclient::pclient)
set_target_properties(pclient_main PROPERTIES OUTPUT_NAME postgres_client)

add_test(NAME pclient.main.can_display_help COMMAND pclient_main --help)
add_test(NAME pclient.main.can_display_version COMMAND pclient_main --version)

add_gtest_for(args)

if (pclient_WANT_INSTALLER)
    install(
        TARGETS pclient_main
        EXPORT pclient_apps
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT pclient_apps)
endif ()
