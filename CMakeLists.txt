cmake_minimum_required(VERSION 3.0)

project(Bookapp VERSION 0.1
  LANGUAGES C)

set(THIRDPARTY_DIR "${PROJECT_SOURCE_DIR}/thirdparty")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("${THIRDPARTY_DIR}")

foreach(SUBDIR dialog sqlite3)
  include_directories("${THIRDPARTY_DIR}/${SUBDIR}")
  link_directories("${THIRDPARTY_DIR}/${SUBDIR}")
endforeach()

add_library(books STATIC "books.c")

add_executable(bookapp main.c)
target_link_libraries(bookapp sqlite3 dialog books ncurses m pthread dl)
add_dependencies(bookapp compile_dialog)

