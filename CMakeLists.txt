cmake_minimum_required( VERSION 3.25 )

if(DEFINED CMAKE_OSX_SYSROOT AND NOT EXISTS "${CMAKE_OSX_SYSROOT}")
  unset(CMAKE_OSX_SYSROOT CACHE)
endif()

message( STATUS "Parallel Programming Course (PPC)" )
project(parallel_programming_course)

############################ Scoreboard #############################

message( STATUS "PPC step: Setup scoreboard generator" )
include(cmake/scoreboard.cmake)
add_subdirectory(scoreboard)

########################### Documentation ###########################

message( STATUS "PPC step: Setup documentation generation" )
include(cmake/sphinx.cmake)
add_subdirectory(docs)

if( USE_SCOREBOARD OR USE_DOCS )
    return()
endif()

############################ Configures #############################

message( STATUS "PPC step: First configures" )
include(cmake/configure.cmake)
include(cmake/modes.cmake)
include(cmake/sanitizers.cmake)
foreach(dep json libenvpp stb)
    include(cmake/${dep}.cmake)
endforeach()

################# Parallel programming technologies #################

message( STATUS "PPC step: Setup parallel programming technologies" )
foreach(dep mpi openmp onetbb)
    include(cmake/${dep}.cmake)
endforeach()

######################### External projects #########################

message( STATUS "PPC step: Setup external projects" )
include(cmake/gtest.cmake)

############################## Modules ##############################

message( STATUS "PPC step: Setup modules" )
add_subdirectory(modules)
add_subdirectory(tasks)
