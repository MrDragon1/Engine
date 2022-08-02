cmake_minimum_required(VERSION 3.23)

project(Ethereal)
set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(ProjectRootDir "${CMAKE_CURRENT_SOURCE_DIR}")
set(EngineRootDir "${ProjectRootDir}")
set(EngineRuntimeDir "${EngineRootDir}/Ethereal")
set(EngineEditorDir "${EngineRootDir}/Editor")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message("EngineRootDir " ${EngineRootDir})
message("EngineRuntimeDir " ${EngineRuntimeDir})
message("EngineEditorDir " ${EngineEditorDir})


add_subdirectory(Ethereal)
add_subdirectory(Editor)