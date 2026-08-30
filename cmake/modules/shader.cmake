include_guard()

function(add_slang_shader_target TARGET)
  cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})

  set(SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
  set(ENTRY_POINTS -entry Vert_main -entry Frag_main)

  if(NOT SLANGC_EXECUTABLE)
      find_program(SLANGC_EXECUTABLE NAMES slangc REQUIRED)
  endif()

  file(MAKE_DIRECTORY ${SHADERS_DIR})

  add_custom_command(
          OUTPUT  ${SHADERS_DIR}/${TARGET}.spv
          COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o ${TARGET}.spv
          WORKING_DIRECTORY ${SHADERS_DIR}
          DEPENDS ${SHADER_SOURCES}
          COMMENT "Compiling Slang Shaders"
          VERBATIM
  )

  add_custom_target(${TARGET} DEPENDS ${SHADERS_DIR}/${TARGET}.spv)
endfunction()
