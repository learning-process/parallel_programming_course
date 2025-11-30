include_guard()

option(USE_SCOREBOARD "Enable scoreboard generation" OFF)
if(USE_SCOREBOARD)
  find_package(Python REQUIRED COMPONENTS Interpreter)
endif(USE_SCOREBOARD)
