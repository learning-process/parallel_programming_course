# Build googletest components
execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -S ${CMAKE_SOURCE_DIR}/3rdparty/googletest
        -B ${CMAKE_BINARY_DIR}/googletest-build
        -D CMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/googletest-install
        -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        RESULT_VARIABLE result)
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}/googletest-build"  --config ${CMAKE_BUILD_TYPE} --parallel
        RESULT_VARIABLE result)
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()
execute_process(
        COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}/googletest-build"
        RESULT_VARIABLE result)
if(result)
    message(FATAL_ERROR "Install step for googletest failed: ${result}")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include_directories(${CMAKE_BINARY_DIR}/googletest-install/include)
