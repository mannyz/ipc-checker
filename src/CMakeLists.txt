set(ACTORS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/actors)
set(COMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common)
set(CONNECTION_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/connection)
set(GENERATOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generator)
set(INTERFACE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/interface)
set(MEASURER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/measurer)
set(RUN_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/run)
set(STATISTICS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/statistics)

add_subdirectory(actors)
add_subdirectory(connection)
add_subdirectory(generator)
add_subdirectory(measurer)
add_subdirectory(run)
add_subdirectory(statistics)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${RUN_SOURCE_DIR}/include
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  run
)
