list(
  APPEND
  MEASURER_SOURCES_LIST
  src/measurers/chrono/measurer.cpp
  src/measurers/rdtsc/measurer.cpp
  src/measurers/rdtsc/calibrator.cpp
)

add_library(measurer STATIC ${MEASURER_SOURCES_LIST})

target_include_directories(
  measurer
  PRIVATE
  #
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/interface/include
)
