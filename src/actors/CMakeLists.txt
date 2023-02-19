list(
  APPEND
  ACTORS_TESTS_SOURCES_LIST
  src/consumer_tests.cpp
)

add_executable(actors_tests ${ACTORS_TESTS_SOURCES_LIST})

target_include_directories(
  actors_tests
  PRIVATE
  #
  ${CONNECTION_SOURCE_DIR}/interface/include
  ${INTERFACE_SOURCE_DIR}/include
  ${MEASURER_SOURCE_DIR}/interface/include
  #
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(actors_tests PRIVATE gmock gtest gtest_main pthread)

add_test(NAME actors_tests COMMAND actors_tests)
