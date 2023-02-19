list(
  APPEND
  STATISTICS_SOURCES_LIST
  src/statistics.cpp
  src/quantiles/src/quantiles.cpp
)

add_library(statistics STATIC ${STATISTICS_SOURCES_LIST})

target_include_directories(
  statistics
  PRIVATE
  #
  ${CONNECTION_SOURCE_DIR}/interface/include
  ${INTERFACE_SOURCE_DIR}/include
  ${MEASURER_SOURCE_DIR}/interface/include
  #
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/quantiles/include
)


list(
  APPEND
  STATISTICS_TESTS_SOURCES_LIST
  src/quantiles/src/tests.cpp
)

add_executable(statistics_tests ${STATISTICS_TESTS_SOURCES_LIST})

target_include_directories(
  statistics_tests
  PRIVATE
  #
  ${CMAKE_CURRENT_SOURCE_DIR}/src/quantiles/include
)

target_link_libraries(statistics_tests PRIVATE statistics)
target_link_libraries(statistics_tests PRIVATE gtest gtest_main)

add_test(NAME statistics_tests COMMAND statistics_tests)