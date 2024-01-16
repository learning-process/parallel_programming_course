get_filename_component(Project_ID ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(Project_ID "sample_${Project_ID}")
message( STATUS "-- " ${Project_ID} )

if(USE_OMP)
    add_executable( ${Project_ID} main.cpp )
    target_link_libraries(${Project_ID} PUBLIC ${OpenMP_libomp_LIBRARY})
endif()