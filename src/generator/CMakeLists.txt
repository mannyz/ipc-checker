list(
  APPEND
  GENERATOR_SOURCES_LIST
  src/simple_round_generator.cpp
  src/simple_round_generator_impl_traits.cpp
)

add_library(generator STATIC ${GENERATOR_SOURCES_LIST})

target_include_directories(
  generator
  PRIVATE
  #
  ${INTERFACE_SOURCE_DIR}/include
  #
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/interface/include
)
