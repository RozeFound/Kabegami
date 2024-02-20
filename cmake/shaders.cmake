function(add_shader TARGET INPUT_PATH OUTPUT_PATH)
  find_program(GLSLC glslc)

  # Add a custom command to compile GLSL to SPIR-V.
  cmake_path(GET OUTPUT_PATH PARENT_PATH OUTPUT_DIR)
  file(MAKE_DIRECTORY ${OUTPUT_DIR})

  add_custom_command(
    OUTPUT ${OUTPUT_PATH}
    COMMAND ${GLSLC} -o ${OUTPUT_PATH} ${INPUT_PATH}
    DEPENDS ${INPUT_PATH}
    IMPLICIT_DEPENDS CXX ${INPUT_PATH}
    VERBATIM)

  # Make sure our build depends on this output.
  set_source_files_properties(${OUTPUT_PATH} PROPERTIES GENERATED TRUE)
  target_sources(${TARGET} PRIVATE ${OUTPUT_PATH})
endfunction(add_shader)