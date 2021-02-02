cmake_minimum_required(VERSION 3.16)

project(
    Postgres_Client
    VERSION 0.0.1
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("${PROJECT_SOURCE_DIR}")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(utils)

# ######################################################################################
# Options
# ######################################################################################
option(BUILD_SHARED_LIBS "Build libraries as .dll's/.so's" ON)
mark_as_advanced(CLEAR BUILD_SHARED_LIBS)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo)
# ######################################################################################
# Subdirectories
# ######################################################################################
add_subdirectory(3rdParty)
add_subdirectory(postgres_client)
