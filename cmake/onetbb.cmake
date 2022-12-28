# Build Core OneTBB components
execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -S ${CMAKE_SOURCE_DIR}/3rdparty/onetbb
        -B ${CMAKE_BINARY_DIR}/onetbb-build
        -D CMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/onetbb-install
        -D TBB_TEST=OFF
        -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -D CMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -D CMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS}
        -D CMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS}
        -D CMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -D CMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS}
        -D CMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS}
        -D CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        RESULT_VARIABLE result)
if(result)
    message(FATAL_ERROR "CMake step for onetbb failed: ${result}")
endif()
execute_process(
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}/onetbb-build" --config ${CMAKE_BUILD_TYPE} --parallel
        RESULT_VARIABLE result)
if(result)
    message(FATAL_ERROR "Build step for onetbb failed: ${result}")
endif()
execute_process(
        COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}/onetbb-build"
        RESULT_VARIABLE result)
if(result)
    message(FATAL_ERROR "Install step for onetbb failed: ${result}")
endif()

include_directories(${CMAKE_BINARY_DIR}/onetbb-install/include)
