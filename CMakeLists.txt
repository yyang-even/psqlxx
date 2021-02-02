cmake_minimum_required(VERSION 3.16)

project(
    Postgres_Client
    VERSION 0.0.1
    LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(utils)

include_directories("${PROJECT_SOURCE_DIR}")

# ######################################################################################
# Options
# ######################################################################################
mark_as_advanced(CLEAR BUILD_SHARED_LIBS)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo)

# ######################################################################################
# Subdirectories
# ######################################################################################
add_subdirectory(3rdParty)
add_subdirectory(postgres_client)
