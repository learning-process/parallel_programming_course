# Build Core Boost components
SUBDIRLIST(subdirs ${CMAKE_SOURCE_DIR}/3rdparty/boost/libs)
foreach(subd ${subdirs})
        include_directories(${CMAKE_SOURCE_DIR}/3rdparty/boost/libs/${subd}/include)
endforeach()

if (MSVC)
        include(ExternalProject)
        ExternalProject_Add(ppc_boost
                SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3rdparty/boost"
                PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/ppc_boost"
                BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/ppc_boost/build"
                INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/ppc_boost/install"
                CONFIGURE_COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}/3rdparty/boost/" -B "${CMAKE_CURRENT_BINARY_DIR}/ppc_boost/build/"
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -G${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBOOST_ENABLE_MPI=ON
                BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_boost/build" --config ${CMAKE_BUILD_TYPE}
                INSTALL_COMMAND   "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/ppc_boost/build" --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_boost/install"
                TEST_COMMAND      "")
else ()
        add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/boost)
        if( USE_MPI )
                find_package( MPI )
                if( MPI_FOUND )
                        set(BOOST_ENABLE_MPI ON)
                        add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/boost/libs/mpi)
                else( MPI_FOUND )
                        set( USE_MPI OFF )
                endif( MPI_FOUND )
        endif( USE_MPI )
endif ()
