include_guard()

if(WIN32 AND CMAKE_GENERATOR MATCHES "Ninja")
  set(CMAKE_NINJA_FORCE_RESPONSE_FILE
      ON
      CACHE BOOL "Force Ninja to use response files" FORCE)
  set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES
      ON
      CACHE BOOL "Use response files for includes" FORCE)
  set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS
      ON
      CACHE BOOL "Use response files for objects" FORCE)
  set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES
      ON
      CACHE BOOL "Use response files for libraries" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

if(MSVC)
  add_compile_options("/utf-8")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/arch")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/arch")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/arch")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

option(USE_COVERAGE "Enable coverage instrumentation" OFF)
option(PPC_EXTERNAL_PROJECTS_VERBOSE
       "Show full configure/build/install logs for ExternalProject dependencies"
       OFF)

if(PPC_EXTERNAL_PROJECTS_VERBOSE)
  set(PPC_EXTERNAL_PROJECT_LOG_ARGS "")
  set(PPC_EXTERNAL_PROJECT_CMAKE_ARGS "")
else()
  set(PPC_EXTERNAL_PROJECT_LOG_ARGS
      LOG_CONFIGURE
      ON
      LOG_BUILD
      ON
      LOG_INSTALL
      ON
      LOG_OUTPUT_ON_FAILURE
      ON)
  set(PPC_EXTERNAL_PROJECT_CMAKE_ARGS -DCMAKE_INSTALL_MESSAGE=NEVER)
endif()

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}/ppc_onetbb/install/lib")

set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
set(CMAKE_SKIP_BUILD_RPATH OFF)

if(UNIX)
  add_compile_options(-Wall -Wextra -Wsign-compare)

  if(NOT APPLE)
    add_compile_options(
      -Wpedantic
      -Wpointer-arith
      -Wwrite-strings
      -Wswitch-enum
      -Wnull-dereference
      -Wformat=2
      -Wmissing-declarations
      -Wno-c11-extensions
      -Wno-cast-function-type)
  endif(NOT APPLE)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wold-style-definition>)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wmissing-prototypes>)

  if("${ENABLE_ADDRESS_SANITIZER}"
     OR "${ENABLE_UB_SANITIZER}"
     OR "${ENABLE_LEAK_SANITIZER}")
    add_compile_options(-Wno-cast-align -Wno-cast-function-type)
  endif()

  if(USE_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
  endif(USE_COVERAGE)
endif()

if(MSVC)
  add_compile_options(/W4 /wd4267 /wd4244)
endif(MSVC)

find_package(Threads REQUIRED)
