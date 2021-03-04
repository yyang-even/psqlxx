# add_gtest_for
function (add_gtest_for source_name)
    if (pclient_WANT_TESTS)
        set(test_source_name "${source_name}.test")
        set(target_name "pclient.${source_name}.")
        add_executable(${target_name} ${test_source_name}.cpp)
        target_link_libraries(${target_name} PRIVATE gtest_main pclient::pclient)

        gtest_discover_tests(
            ${target_name}
            TEST_PREFIX ${target_name}
            PROPERTIES LABELS ${PROJECT_NAME})

        if (pclient_WANT_AUTO_TESTS)
            add_custom_command(
                TARGET ${target_name}
                POST_BUILD
                COMMAND ctest --output-on-failure -R ^${target_name}
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

    include(CheckIPOSupported)
    check_ipo_supported(RESULT result)
    if (result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    endif ()
endmacro ()
