cmake_minimum_required(VERSION 3.0)

project(BackendApp VERSION 0.1
  LANGUAGES C)

# Configure dialog.
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/dialog/config.status")
  execute_process(COMMAND ./configure
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/dialog"
    RESULT_VARIABLE DIALOG_COMPILATION_RESULT)
  if(NOT DIALOG_COMPILATION_RESULT EQUAL "0")
    message(FATAL_ERROR "Configuration failed. Aboring.\nPlease read the INSTALL file.")
  endif()
endif()

# Compile dialog.
add_custom_command(OUTPUT "${PROJECT_SOURCE_DIR}/dialog/libdialog.a"
  COMMAND make -j$(nproc) libdialog.a
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/dialog")
add_custom_target(compile_dialog ALL
  DEPENDS "${PROJECT_SOURCE_DIR}/dialog/libdialog.a")

include_directories("${PROJECT_SOURCE_DIR}/dialog")
link_directories("${PROJECT_SOURCE_DIR}/dialog")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(backendapp main.c)
target_link_libraries(backendapp libdialog.a ncurses m)
add_dependencies(backendapp compile_dialog)
