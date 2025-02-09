cmake_minimum_required( VERSION 3.25 )

message( STATUS "Parallel Programming Course (PPC)" )
project(parallel_programming_course)

############################ Configures #############################

message( STATUS "PPC step: First configures" )
include(cmake/configure.cmake)
include(cmake/modes.cmake)
include(cmake/sanitizers.cmake)

################# Parallel programming technologies #################

message( STATUS "PPC step: Setup parallel programming technologies" )
include(cmake/mpi.cmake)
include(cmake/openmp.cmake)
include(cmake/threads.cmake)
include(cmake/onetbb.cmake)

######################### External projects #########################

message( STATUS "PPC step: Setup external projects" )
include(cmake/opencv.cmake)
include(cmake/gtest.cmake)

########################### Documentation ###########################

message( STATUS "PPC step: Setup documentation generation" )
include(cmake/sphinx.cmake)
add_subdirectory(docs)

############################ Scoreboard #############################

message( STATUS "PPC step: Setup scoreboard generator" )
include(cmake/scoreboard.cmake)
add_subdirectory(scoreboard)

############################## Headers ##############################

message( STATUS "PPC step: Setup headers" )
include_directories(3rdparty)
include_directories(modules)
include_directories(tasks)

############################## Modules ##############################

message( STATUS "PPC step: Setup modules" )
add_subdirectory(modules)
add_subdirectory(tasks)
