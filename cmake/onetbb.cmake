# Build Core OneTBB components
option(TBB_TEST "Enable testing" OFF)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/onetbb/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/onetbb)
