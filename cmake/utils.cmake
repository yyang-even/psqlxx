# add_gtest_for
function (add_gtest_for source_name)
    if (WANT_TESTS)
        set(target_name "${source_name}.test")
        add_executable(${target_name} ${target_name}.cpp ${source_name}.cpp)
        target_link_libraries(${target_name} PRIVATE gtest_main ${PTHREAD_LIBRARY}
                                                     ${ARGN})
        add_test(NAME ${target_name} COMMAND ${target_name})

        if (WANT_AUTO_TESTS)
            add_custom_command(
                TARGET ${target_name}
                POST_BUILD
                COMMAND ctest --output-on-failure -R ^${target_name}$
                COMMENT "Testing '${target_name}'"
                VERBATIM)
        endif ()
    endif ()
endfunction ()

# config_compiler_and_linker
macro (CONFIG_CXX_COMPILER_AND_LINKER)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

endmacro ()
