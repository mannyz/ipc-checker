cmake_minimum_required(VERSION 3.18.0)

project(ipc-checker VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)

set(PROGRAM_BUG_ADDRESS "<a.filomaphitskay@gmail.com>")
add_compile_definitions(IPC_CHECKER_PROGRAM_NAME="${PROJECT_NAME}")
add_compile_definitions(
  IPC_CHECKER_PROGRAM_VERSION="${PROJECT_NAME} ${PROJECT_VERSION}")
add_compile_definitions(
  IPC_CHECKER_PROGRAM_BUG_ADDRESS="${PROGRAM_BUG_ADDRESS}")

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
