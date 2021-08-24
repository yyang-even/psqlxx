find_program(BASH_PROGRAM bash)

if (BASH_PROGRAM)
    add_test(format_tests ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/format_tests.sh)
endif (BASH_PROGRAM)
