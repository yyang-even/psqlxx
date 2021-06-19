find_program(BASH_PROGRAM bash REQUIRED)

configure_file(test_db_defines.sh.in test_db_defines.sh @ONLY)
configure_file(test_db_defines.py.in test_db_defines.py @ONLY)

add_test(NAME psqlxx.real_db.setup COMMAND ${BASH_PROGRAM}
                                           ${CMAKE_CURRENT_SOURCE_DIR}/test_db_setup.sh)
set_tests_properties(psqlxx.real_db.setup PROPERTIES FIXTURES_SETUP RealDbTests)

add_test(NAME psqlxx.real_db.cleanup
         COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/test_db_cleanup.sh)
set_tests_properties(psqlxx.real_db.cleanup PROPERTIES FIXTURES_CLEANUP RealDbTests)

add_test(NAME psqlxx.real_db.psql_diff
         COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/test_psql_diff.py)
set_tests_properties(psqlxx.real_db.psql_diff PROPERTIES FIXTURES_REQUIRED RealDbTests)
