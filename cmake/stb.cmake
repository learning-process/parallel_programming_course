include_guard()

function(ppc_link_stb exec_func_lib)
  set(stb_image_header ${CMAKE_SOURCE_DIR}/3rdparty/stb/stb_image.h)

  add_library(stb_image STATIC ${stb_image_header})
  set_source_files_properties(${stb_image_header} PROPERTIES LANGUAGE C)
  set_target_properties(stb_image PROPERTIES EXPORT_COMPILE_COMMANDS OFF)
  target_compile_definitions(stb_image PRIVATE STB_IMAGE_IMPLEMENTATION)
  target_include_directories(stb_image PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty)
  target_link_libraries(${exec_func_lib} PUBLIC stb_image)
endfunction()
