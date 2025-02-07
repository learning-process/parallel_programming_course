option(USE_SCOREBOARD OFF)
if( USE_SCOREBOARD )
    find_package(Python REQUIRED
        COMPONENTS Interpreter)
endif( USE_SCOREBOARD )
