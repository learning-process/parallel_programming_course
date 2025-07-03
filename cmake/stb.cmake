function(ppc_link_stb exec_func_lib)
  add_library(stb_image STATIC
              ${CMAKE_SOURCE_DIR}/3rdparty/stb_image_wrapper.cpp)
  target_include_directories(stb_image PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/stb)
  target_link_libraries(${exec_func_lib} PUBLIC stb_image)
endfunction()
