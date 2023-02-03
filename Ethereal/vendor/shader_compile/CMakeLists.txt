set(TARGET_NAME ${SHADER_COMPILE_TARGET})
set(SHADER_SOURCE_PATH "${EngineEditorDir}/assets/shaders")

file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "${SHADER_SOURCE_PATH}/**.vert"
                                            "${SHADER_SOURCE_PATH}/**.frag"
                                            "${SHADER_SOURCE_PATH}/**.geom")

SOURCE_GROUP("shaders" FILES ${SHADER_FILES})

set(SHADER_INCLUDE_FOLDER ${SHADER_SOURCE_PATH}/Include)

set(GENERATED_SHADER_FOLDER "src/Resource/generated")
if (NOT EXISTS ${EngineRuntimeDir}/${GENERATED_SHADER_FOLDER})
  file(MAKE_DIRECTORY ${EngineRuntimeDir}/${GENERATED_SHADER_FOLDER})
endif()

if (NOT EXISTS ${EngineRuntimeDir}/${GENERATED_SHADER_FOLDER}/spv)
  file(MAKE_DIRECTORY ${EngineRuntimeDir}/${GENERATED_SHADER_FOLDER}/spv)
endif()

include(ShaderCompile.cmake)

compile_shader(
  "${SHADER_FILES}"
  "${TARGET_NAME}"
  "${SHADER_INCLUDE_FOLDER}"
  "${GENERATED_SHADER_FOLDER}"
  "${CMAKE_CURRENT_SOURCE_DIR}/glslangValidator.exe")

set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "Engine" )
