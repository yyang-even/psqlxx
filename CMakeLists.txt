cmake_minimum_required(VERSION 3.16)

project(
    Postgres_Client
    VERSION 0.0.1
    LANGUAGES C CXX)

find_package(Git REQUIRED)
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --long --abbrev=7
    RESULT_VARIABLE result
    OUTPUT_VARIABLE GIT_COMMIT_DESCRIPTION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if (result)
    message(WARNING "Failed to get Git description: ${result}")
endif ()

include_directories("${PROJECT_SOURCE_DIR}")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(utils)

config_cxx_compiler_and_linker()

# ######################################################################################
# CMake Standard Options
option(BUILD_SHARED_LIBS "Build libraries as .dll's/.so's." ON)
mark_as_advanced(CLEAR BUILD_SHARED_LIBS)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo)

# ######################################################################################
# Project Options
option(pclient_WANT_TESTS "Build all of the project's own tests." ON)
option(pclient_WANT_AUTO_TESTS "Automatically run tests that have been changed." ON)

if (pclient_WANT_TESTS)
    enable_testing()
    include(CTest)
    mark_as_advanced(BUILD_TESTING)

    include(GoogleTest)
endif ()

option(pclient_WANT_INSTALLER "Build the project's own installer." OFF)

if (pclient_WANT_INSTALLER)
    include(GNUInstallDirs)
    set(CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME})

    file(RELATIVE_PATH relDir ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
         ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
    set(CMAKE_INSTALL_RPATH $ORIGIN $ORIGIN/${relDir})
endif ()

# ######################################################################################
# Subdirectories
add_subdirectory(3rdParty)
add_subdirectory(postgres_client)
