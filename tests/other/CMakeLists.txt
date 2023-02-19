list(
  APPEND
  OTHER_TESTS_SOURCES_LIST
  src/worker_tests.cpp
)

add_executable(other_tests ${OTHER_TESTS_SOURCES_LIST})

target_include_directories(
  other_tests
  PRIVATE
  ${INTERFACE_SOURCE_DIR}/include
)

target_link_libraries(other_tests PRIVATE gmock gtest gtest_main pthread)

add_test(NAME other_tests COMMAND other_tests)
