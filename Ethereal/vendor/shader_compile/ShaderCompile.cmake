function(compile_shader SHADERS TARGET_NAME SHADER_INCLUDE_FOLDER GENERATED_DIR GLSLANG_BIN)
    MESSAGE(STATUS "Compiling shaders for ${TARGET_NAME}...")

#    MESSAGE(STATUS "Shader files: ${SHADERS}")
#    MESSAGE(STATUS "Target name: ${TARGET_NAME}")
#    MESSAGE(STATUS "Shader include folder: ${SHADER_INCLUDE_FOLDER}")
#    MESSAGE(STATUS "Generated shader folder: ${GENERATED_DIR}")
#    MESSAGE(STATUS "glslangValidator_executable: ${GLSLANG_BIN}")

    set(ALL_GENERATED_SPV_FILES "")
    set(ALL_GENERATED_CPP_FILES "")
    set(ALL_GENERATED_CPP_FILES_INCLUDES "")

    foreach(SHADER ${SHADERS})
    # Prepare a header name and a global variable for this shader
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
        string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)

        set(SPV_FILE "${EngineRuntimeDir}/${GENERATED_DIR}/spv/${SHADER_NAME}.spv")
        set(CPP_FILE "${EngineRuntimeDir}/${GENERATED_DIR}/cpp/${HEADER_NAME}.h")

        execute_process(COMMAND ${GLSLANG_BIN} -I${SHADER_INCLUDE_FOLDER} -V100 -o ${SPV_FILE} ${SHADER} --target-env opengl
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                TIMEOUT 3
                RESULT_VARIABLE result_var
                OUTPUT_VARIABLE output_var
                ERROR_VARIABLE error_var
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_STRIP_TRAILING_WHITESPACE)

        if(result_var)
            message(STATUS "result: ${result_var}")
            message(STATUS "output: ${output_var}")
            message(STATUS "error: ${error_var}")
        endif()

        list(APPEND ALL_GENERATED_SPV_FILES ${SPV_FILE})
        execute_process(COMMAND cmake -DPATH=${SPV_FILE} -DHEADER=${CPP_FILE}
                -DGLOBAL="${GLOBAL_SHADER_VAR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/GenerateShaderCPPFile.cmake"
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                TIMEOUT 3
                RESULT_VARIABLE result_var
                OUTPUT_VARIABLE output_var
                ERROR_VARIABLE error_var
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_STRIP_TRAILING_WHITESPACE)
        if(result_var)
            message(STATUS "result: ${result_var}")
            message(STATUS "output: ${output_var}")
            message(STATUS "error: ${error_var}")
        endif()

        list(APPEND ALL_GENERATED_CPP_FILES ${CPP_FILE})
        set(includetmp "#include \"${CPP_FILE}\"\n")
        string(APPEND ALL_GENERATED_CPP_FILES_INCLUDES ${includetmp})

    endforeach()
    set(source "/**\n * @file ShaderBinaryData.h\n * @brief Auto generated file.\n */\n${ALL_GENERATED_CPP_FILES_INCLUDES}\n")
    file(WRITE "${EngineRuntimeDir}/${GENERATED_DIR}/cpp/ShaderBinaryData.h" "#pragma once\n\n${ALL_GENERATED_CPP_FILES_INCLUDES}")

    add_custom_target(${TARGET_NAME}
        DEPENDS ${ALL_GENERATED_SPV_FILES} ${ALL_GENERATED_CPP_FILES} SOURCES ${SHADERS})

    MESSAGE(STATUS "Compiling shaders for ${TARGET_NAME}... Done")
endfunction()