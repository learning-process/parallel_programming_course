cmake_minimum_required( VERSION 2.8 )

message( STATUS "Parallel Programming Course" )

find_package( MPI )
if( MPI_FOUND )
    include_directories( ${MPI_INCLUDE_PATH} )
endif( MPI_FOUND )

if(CMAKE_${LANG}_COMPILER_ID STREQUAL "AppleClang"
      AND CMAKE_${LANG}_COMPILER_VERSION VERSION_GREATER_EQUAL "7.0")
      find_library(OpenMP_libomp_LIBRARY
        NAMES omp gomp iomp5
        HINTS ${CMAKE_${LANG}_IMPLICIT_LINK_DIRECTORIES}
      )
      mark_as_advanced(OpenMP_libomp_LIBRARY)

      if(OpenMP_libomp_LIBRARY)
        try_compile( OpenMP_COMPILE_RESULT_${FLAG_MODE}_${OPENMP_PLAIN_FLAG} ${CMAKE_BINARY_DIR} ${_OPENMP_TEST_SRC}
          CMAKE_FLAGS "-DCOMPILE_DEFINITIONS:STRING=${OPENMP_FLAGS_TEST}"
          LINK_LIBRARIES ${CMAKE_${LANG}_VERBOSE_FLAG} ${OpenMP_libomp_LIBRARY}
          OUTPUT_VARIABLE OpenMP_TRY_COMPILE_OUTPUT
        )
        if(OpenMP_COMPILE_RESULT_${FLAG_MODE}_${OPENMP_PLAIN_FLAG})
          set("${OPENMP_FLAG_VAR}" "${OPENMP_FLAG}" PARENT_SCOPE)
          set("${OPENMP_LIB_NAMES_VAR}" "libomp" PARENT_SCOPE)
          break()
        endif()
    endif()
endif()

find_package( OpenMP )
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

if ( UNIX )
    set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif( UNIX )

if ( MSVC )
    set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} /WX")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /WX")
endif( MSVC )

add_subdirectory(modules)
