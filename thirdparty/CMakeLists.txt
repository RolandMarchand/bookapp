# Configure dialog.
# message(FATAL_ERROR ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/dialog/config.status")
  execute_process(COMMAND ./configure
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dialog"
    RESULT_VARIABLE DIALOG_COMPILATION_RESULT)
  if(NOT DIALOG_COMPILATION_RESULT EQUAL "0")
    message(FATAL_ERROR "Configuration failed. Aboring.\nPlease read the INSTALL file.")
  endif()
endif()

# Compile dialog.
add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/dialog/libdialog.a"
  COMMAND make -j libdialog.a
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dialog")
add_custom_target(compile_dialog ALL
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/dialog/libdialog.a")

add_library(sqlite3 STATIC "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3/sqlite3.c")

