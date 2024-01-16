get_filename_component(Project_ID ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(Project_ID "sample_${Project_ID}")
message( STATUS "-- " ${Project_ID} )

if(USE_MPI)
    add_executable( ${Project_ID} main.cpp )

    if (MPI_COMPILE_FLAGS)
        set_target_properties( ${Project_ID} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}" )
    endif (MPI_COMPILE_FLAGS)

    if (MPI_LINK_FLAGS)
        set_target_properties( ${Project_ID} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}" )
    endif (MPI_LINK_FLAGS)

    target_link_libraries(${Project_ID} PUBLIC ${MPI_LIBRARIES})
    add_dependencies(${Project_ID} ppc_boost)
    target_link_directories(${Project_ID} PUBLIC ${CMAKE_BINARY_DIR}/ppc_boost/install/lib)
    if (NOT MSVC)
        target_link_libraries(${Project_ID} PUBLIC boost_mpi)
    endif ()
endif()
