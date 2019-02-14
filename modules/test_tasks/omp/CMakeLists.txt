project( test_omp )

if ( OpenMP_FOUND )
    message( STATUS "-- " ${PROJECT_NAME} )
    add_executable( ${PROJECT_NAME} main.cpp )
else( OpenMP_FOUND )
    message( STATUS "-- ${PROJECT_NAME} not build!"  )
endif( OpenMP_FOUND )
