find_program(BASH_PROGRAM bash REQUIRED)

add_test(NAME psqlxx.db.setup COMMAND ${BASH_PROGRAM}
                                      ${CMAKE_CURRENT_SOURCE_DIR}/test_db_setup.sh)
set_tests_properties(psqlxx.db.setup PROPERTIES FIXTURES_SETUP DbTestSuite)

add_test(NAME psqlxx.db.cleanup COMMAND ${BASH_PROGRAM}
                                        ${CMAKE_CURRENT_SOURCE_DIR}/test_db_cleanup.sh)
set_tests_properties(psqlxx.db.cleanup PROPERTIES FIXTURES_CLEANUP DbTestSuite)
