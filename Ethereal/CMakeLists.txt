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

set(RuntimeSourceFiles "")
add_subdirectory(src)
add_subdirectory(vendor)

source_group(TREE "${EngineRuntimeDir}" FILES ${RuntimeSourceFiles})

add_library(EtherealRuntime STATIC ${RuntimeSourceFiles})

target_include_directories(EtherealRuntime
        PRIVATE "${EngineRootDir}"
        PRIVATE "${EngineRuntimeDir}/src" # make sure we can directly include "pch.h"
        PRIVATE "${EngineRuntimeDir}/vendor" ) 

target_compile_definitions(EtherealRuntime
        PUBLIC ET_ENABLE_ASSERTS
        PRIVATE GLFW_INCLUDE_NONE
        PUBLIC ET_PLATFORM_WINDOWS
        PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM
	PUBLIC IMGUI_DEFINE_MATH_OPERATORS
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
#find_package(imgui CONFIG REQUIRED) # imgui[docking-experimental]:x64-windows
#find_package(imguizmo CONFIG REQUIRED) # x64-windows
find_package(pugixml CONFIG REQUIRED)

find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")

# Vulkan SDK设置
if(NOT DEFINED ENV{VK_SDK_PATH})
    message(FATAL_ERROR "VK_SDK_PATH not defined!")
endif()
set(VK_SDK_PATH $ENV{VK_SDK_PATH})
set(VK_SDK_INCLUDE ${VK_SDK_PATH}/Include)
set(VK_SDK_LIB ${VK_SDK_PATH}/Lib/vulkan-1.lib)

set(SPIRV_CROSS_LIB ${VK_SDK_PATH}/Lib/spirv-cross-cored.lib ${VK_SDK_PATH}/Lib/spirv-cross-glsld.lib)
set(GLSLANG_LIB 
${VK_SDK_PATH}/Lib/glslangd.lib 
${VK_SDK_PATH}/Lib/SPIRVd.lib 
${VK_SDK_PATH}/Lib/MachineIndependentd.lib 
${VK_SDK_PATH}/Lib/glslang-default-resource-limitsd.lib 
${VK_SDK_PATH}/Lib/GenericCodeGend.lib 
${VK_SDK_PATH}/Lib/SPVRemapperd.lib 
${VK_SDK_PATH}/Lib/OSDependentd.lib 
${VK_SDK_PATH}/Lib/OGLCompilerd.lib 
${VK_SDK_PATH}/Lib/HLSLd.lib 
${VK_SDK_PATH}/Lib/SPIRV-Toolsd.lib 
${VK_SDK_PATH}/Lib/SPIRV-Tools-sharedd.lib 
${VK_SDK_PATH}/Lib/SPIRV-Tools-reduced.lib 
${VK_SDK_PATH}/Lib/SPIRV-Tools-lintd.lib 
${VK_SDK_PATH}/Lib/SPIRV-Tools-linkd.lib 
${VK_SDK_PATH}/Lib/SPIRV-Tools-optd.lib 
${VK_SDK_PATH}/Lib/SPIRV-Tools-diffd.lib)

target_include_directories(EtherealRuntime PUBLIC ${STB_INCLUDE_DIRS} ${VK_SDK_INCLUDE})
target_link_libraries(EtherealRuntime PUBLIC glad::glad glm::glm glfw
        assimp::assimp EnTT::EnTT glfw
        spdlog::spdlog spdlog::spdlog_header_only yaml-cpp pugixml::pugixml 
        imgui-docking-layout box2d::box2d ${VK_SDK_LIB} ${GLSLANG_LIB})

