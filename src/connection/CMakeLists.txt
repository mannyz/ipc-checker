list(
  APPEND
  CONNECTION_SOURCES_LIST
  src/buffer/buffer.cpp
  src/connectors/net_socket/client.cpp
  src/connectors/net_socket/server.cpp
  src/connectors/pipe/client.cpp
  src/connectors/pipe/server.cpp
  src/connectors/shared_memory/client.cpp
  src/connectors/shared_memory/ring_buffer.cpp
  src/connectors/shared_memory/server.cpp
  src/connectors/unix_socket/client.cpp
  src/connectors/unix_socket/server.cpp
  src/types.cpp
)

add_library(connection STATIC ${CONNECTION_SOURCES_LIST})

target_include_directories(
  connection
  PRIVATE
  #
  ${COMMON_SOURCE_DIR}/include
  ${INTERFACE_SOURCE_DIR}/include
  #
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/interface/include
)


list(
  APPEND
  CONNECTION_TESTS_SOURCES_LIST
  src/connectors/net_socket/tests.cpp
  src/connectors/pipe/tests.cpp
  src/connectors/shared_memory/tests.cpp
  src/connectors/unix_socket/tests.cpp
  src/packers/tests.cpp
)

add_executable(connection_tests ${CONNECTION_TESTS_SOURCES_LIST})

target_include_directories(
  connection_tests
  PRIVATE
  #
  ${COMMON_SOURCE_DIR}/include
  ${INTERFACE_SOURCE_DIR}/include
  #
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/interface/include
)

target_link_libraries(connection_tests PRIVATE connection)
target_link_libraries(connection_tests PRIVATE gmock gtest gtest_main pthread)

add_test(NAME connection_tests COMMAND connection_tests)
