cmake_minimum_required( VERSION 3.25 )

project(parallel_programming_course)
message( STATUS "Parallel Programming Course" )

include(cmake/configure.cmake)
include(cmake/gtest.cmake)

option(ENABLE_ADDRESS_SANITIZER OFF)
include(cmake/sanitizers.cmake)

################################ MPI ################################
option(USE_MPI OFF)
if( USE_MPI )
    find_package( MPI )
    if( MPI_FOUND )
        include_directories( ${MPI_INCLUDE_PATH} )
    else( MPI_FOUND )
        set( USE_MPI OFF )
    endif( MPI_FOUND )
    include(cmake/boost.cmake)
endif( USE_MPI )

############################### OpenMP ##############################
option(USE_OMP OFF)
if( USE_OMP OR USE_SEQ )
    find_package( OpenMP )
    if( OpenMP_FOUND )
        include_directories( ${OpenMP_C_INCLUDE_DIRS} ${OpenMP_CXX_INCLUDE_DIRS} )
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
        if ( MSVC AND MSVC_VERSION GREATER 1919 )
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /openmp:experimental")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp:experimental")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
            add_compile_options(/openmp:experimental)
        endif()
    else( OpenMP_FOUND )
        if (WIN32)
            message(WARNING "OpenMP NOT FOUND")
        else()
            message(FATAL_ERROR "OpenMP NOT FOUND")
        endif()
    endif( OpenMP_FOUND )
endif( USE_OMP OR USE_SEQ )

############################ std::thread ############################
option(USE_STL OFF)
if( USE_STL )
    find_package( Threads )
endif( USE_STL )

################################ TBB ################################
option(USE_TBB OFF)
if( USE_TBB )
    include(cmake/onetbb.cmake)
endif( USE_TBB )

######################### Functional mode ##########################
option(USE_FUNC_TESTS OFF)
if( USE_FUNC_TESTS )
    message( STATUS "Enable functional tests" )
    add_compile_definitions(USE_FUNC_TESTS)
endif( USE_FUNC_TESTS )

######################### Performance mode ##########################
option(USE_PERF_TESTS OFF)
if( USE_PERF_TESTS )
    message( STATUS "Enable performance tests" )
    add_compile_definitions(USE_PERF_TESTS)
endif( USE_PERF_TESTS )

############################## Modules ##############################

include_directories(3rdparty)
include_directories(modules)
include_directories(tasks)

add_subdirectory(modules)
add_subdirectory(tasks)
add_subdirectory(1stsamples)
