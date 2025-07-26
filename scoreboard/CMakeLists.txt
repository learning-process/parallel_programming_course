if(NOT USE_SCOREBOARD)
  return()
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html)
set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/html)

add_custom_target(
  generate_scoreboard ALL
  COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/main.py -o
          ${OUTPUT_DIR}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running main.py")
