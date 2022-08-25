cmake_minimum_required(VERSION 3.0)

project(BackendApp VERSION 0.1
  LANGUAGES C)

set(THIRDPARTY_DIR "${PROJECT_SOURCE_DIR}/thirdparty")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("${THIRDPARTY_DIR}")

foreach(SUBDIR dialog sqlite3)
  include_directories("${THIRDPARTY_DIR}/${SUBDIR}")
  link_directories("${THIRDPARTY_DIR}/${SUBDIR}")
endforeach()

add_executable(backendapp main.c)
target_link_libraries(backendapp sqlite3 dialog ncurses m)
add_dependencies(backendapp compile_dialog)

