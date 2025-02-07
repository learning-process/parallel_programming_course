if(NOT USE_SEQ AND NOT USE_MPI AND NOT USE_OMP AND NOT USE_TBB AND NOT USE_STL)
  return()
endif()

message(STATUS "Modules")

SUBDIRLIST(subdirs ${CMAKE_CURRENT_SOURCE_DIR})

foreach(subd ${subdirs})
  add_subdirectory(${subd})
endforeach()
