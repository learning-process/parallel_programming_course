# Download and unpack boost at configure time
configure_file(${CMAKE_SOURCE_DIR}/cmake/cmake_configure/boost.txt.in boost-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/boost-download )
if(result)
    message(FATAL_ERROR "CMake step for boost failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/boost-download )
if(result)
    message(FATAL_ERROR "Build step for boost failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add boost directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(
        ${CMAKE_SOURCE_DIR}/3rdparty/boost
        ${CMAKE_BINARY_DIR}/boost-build
        EXCLUDE_FROM_ALL)
