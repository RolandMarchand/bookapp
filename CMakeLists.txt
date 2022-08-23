cmake_minimum_required(VERSION 3.19)

project(BackendApp VERSION 0.1
  DESCRIPTION "A learning experience."
  LANGUAGES C)

# if(NOT EXISTS "${PROJECT_SOURCE_DIR}/libdialog.a")
#   execute_process(
#     COMMAND ./configure
# #    COMMAND make
#     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dialog
#     RESULT_VARIABLE DIALOG_COMPILATION_RESULT
#     COMMAND_ERROR_IS_FATAL ANY
#   )
#   if(NOT DIALOG_COMPILATION_RESULT EQUAL "0")
#     message(FATAL_ERROR "Dialog compilation unsuccessful. Aboring.")
#   endif()
# endif()

execute_process(COMMAND ./configure
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/dialog"
  RESULT_VARIABLE DIALOG_COMPILATION_RESULT
  COMMAND_ERROR_IS_FATAL ANY)

# add_custom_command(OUTPUT /dev/null
#   COMMAND make
#   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dialog)

# if(NOT EXISTS "${PROJECT_SOURCE_DIR}/libdialog.a")
#   message(FATAL_ERROR "Dialog compilation unsuccessful. Aboring.")
# endif()

add_custom_command(OUTPUT "${PROJECT_SOURCE_DIR}/dialog/libdialog.a"
  COMMAND make
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/dialog")

add_custom_target(compile_dialog ALL
  DEPENDS "${PROJECT_SOURCE_DIR}/dialog/libdialog.a")

find_library(DIALOG
  NAMES libdialog.a
  PATHS ${PROJECT_SOURCE_DIR}/dialog)

include_directories("${PROJECT_SOURCE_DIR}/dialog")
add_executable(backendapp main.c)
target_link_libraries(backendapp ${DIALOG} ncurses m)
#link_directories(BEFORE "${PROJECT_SOURCE_DIR}/dialog/")
# link_directories("${PROJECT_SOURCE_DIR}/dialog/libdialog.a")

#target_link_libraries(backendapp PRIVATE ${PROJECT_SOURCE_DIR}/dialog/libdialog.a)
