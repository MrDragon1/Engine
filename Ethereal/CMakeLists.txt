macro(RUNTIME_ADD_SOURCE)
    set(SOURCE_FILES "")
    foreach(SOURCE_FILE ${ARGN})
        if(SOURCE_FILE MATCHES ":")
            list(APPEND SOURCE_FILES ${SOURCE_FILE})
        else()
            list(APPEND SOURCE_FILES
                    "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        endif()
    endforeach()
    set(RuntimeSourceFiles ${RuntimeSourceFiles} ${SOURCE_FILES} PARENT_SCOPE)
endmacro(RUNTIME_ADD_SOURCE)

set(RuntimeSourceFiles "" src/Base/Math/Vector.cpp src/Base/Math/Vector.h src/Base/Meta/Reflection.cpp src/Base/Meta/Reflection.h src/Base/Meta/Serializer.cpp src/Base/Meta/Serializer.h src/Base/Yaml.h src/Base/Meta/ReflectionRegister.cpp src/Base/Meta/ReflectionRegister.h src/Base/Math/Math.cpp src/Base/Math/Math.h src/Base/Math/Random.h src/Base/Math/Matrix.cpp src/Base/Math/Matrix.h src/Base/Math/Quaternion.cpp src/Base/Math/Quaternion.h src/Base/Math/Transform.h src/Base/Meta/Raw/Camera.h src/Base/Meta/Raw/Scene.h src/Core/Scene/Components.cpp src/Resource/ResourceLoader.cpp src/Resource/ResourceLoader.h src/Resource/Descriptor/Descriptor.h src/Resource/Descriptor/Animation.h src/Resource/Descriptor/Mesh.h src/Resource/Descriptor/Material.h src/Resource/ResourceImporter.cpp src/Resource/ResourceImporter.h)

add_subdirectory(src)
add_subdirectory(vendor)

source_group(TREE "${EngineRuntimeDir}" FILES ${RuntimeSourceFiles})
message("Files " ${RuntimeSourceFiles})

add_library(EtherealRuntime STATIC ${RuntimeSourceFiles})

target_include_directories(EtherealRuntime
        PRIVATE "${EngineRootDir}"
        PRIVATE "${EngineRuntimeDir}/src" ) # make sure we can directly include "pch.h")

target_compile_definitions(EtherealRuntime
        PUBLIC ET_ENABLE_ASSERTS
        PRIVATE GLFW_INCLUDE_NONE
        PUBLIC ET_PLATFORM_WINDOWS
        PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM
        PUBLIC ET_DEBUG
        PUBLIC "ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
target_precompile_headers(EtherealRuntime PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")


find_package(glad CONFIG REQUIRED)  # x64-windows
find_package(glm CONFIG REQUIRED)   # x64-windows-static
find_package(glfw3 CONFIG REQUIRED) # x64-windows
find_package(assimp CONFIG REQUIRED) # x64-windows
find_package(EnTT CONFIG REQUIRED) # x64-windows
find_package(spdlog CONFIG REQUIRED) # x64-windows
find_package(yaml-cpp CONFIG REQUIRED) # x64-windows
find_package(box2d CONFIG REQUIRED) # x64-windows (-static)
find_package(imgui CONFIG REQUIRED) # imgui[docking-experimental]:x64-windows
find_package(imguizmo CONFIG REQUIRED) # x64-windows

find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")

target_include_directories(EtherealRuntime PUBLIC ${STB_INCLUDE_DIRS})
target_link_libraries(EtherealRuntime PUBLIC glad::glad glfw glm::glm
        assimp::assimp EnTT::EnTT
        spdlog::spdlog spdlog::spdlog_header_only yaml-cpp
        imgui::imgui imguizmo::imguizmo box2d::box2d)

