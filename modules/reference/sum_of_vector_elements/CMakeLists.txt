get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(ProjectId "${ProjectId}_ref")
message( STATUS "-- " ${ProjectId} )

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.hpp)

set(PACK_LIB "${ProjectId}_lib")
add_library(${PACK_LIB} STATIC ${ALL_SOURCE_FILES} )

add_executable( ${ProjectId} ${ALL_SOURCE_FILES} )

target_link_libraries(${ProjectId} ${PACK_LIB})
target_link_libraries(${ProjectId} core_lib)
target_link_libraries(${ProjectId} gtest gtest_main)

enable_testing()
add_test(NAME ${ProjectId} COMMAND ${ProjectId})

CPPCHECK_AND_COUNTS_TESTS("${ProjectId}" "${ALL_SOURCE_FILES}")
