configure_file(version.cpp.in version.cpp @ONLY)
add_library(version STATIC ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
set_target_properties(version PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(postgres_client main.cpp)
target_link_libraries(postgres_client PRIVATE version)
