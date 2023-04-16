macro(EDITOR_ADD_SOURCE)
    set(SOURCE_FILES "")
    foreach(SOURCE_FILE ${ARGN})
        if(SOURCE_FILE MATCHES ":")
            list(APPEND SOURCE_FILES ${SOURCE_FILE})
        else()
            list(APPEND SOURCE_FILES
                    "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        endif()
    endforeach()
    set(EditorSourceFiles ${EditorSourceFiles} ${SOURCE_FILES} PARENT_SCOPE)
endmacro(EDITOR_ADD_SOURCE)

set(EditorSourceFiles "")
add_subdirectory(src)

source_group(TREE ${EngineEditorDir} FILES ${EditorSourceFiles})

add_executable(EtherealEditor ${EditorSourceFiles})

add_compile_definitions("ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

target_link_libraries(EtherealEditor PRIVATE EtherealRuntime)

target_include_directories(EtherealEditor PRIVATE
        "${EngineRuntimeDir}/vendor"
        "${EngineRuntimeDir}/src"
        "${EngineRuntimeDir}"
        )

target_compile_definitions(EtherealEditor PRIVATE
        ET_PLATFORM_WINDOWS
        _CRT_SECURE_NO_WARNINGS
        )
