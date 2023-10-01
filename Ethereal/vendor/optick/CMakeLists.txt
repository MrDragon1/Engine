project(optick VERSION 1.88)

set(_optick_Sources
"optick.config.h"
"optick.h"
"optick_capi.h"
"optick_common.h"
"optick_core.freebsd.h"
"optick_core.h"
"optick_core.linux.h"
"optick_core.macos.h"
"optick_core.platform.h"
"optick_core.win.h"
"optick_gpu.h"
"optick_memory.h"
"optick_message.h"
"optick_miniz.h"
"optick_serialization.h"
"optick_server.h"
"optick_capi.cpp"
"optick_core.cpp"
"optick_gpu.cpp"
"optick_gpu.d3d12.cpp"
"optick_gpu.vulkan.cpp"
"optick_message.cpp"
"optick_miniz.cpp"
"optick_serialization.cpp"
"optick_server.cpp"
)
# Vulkan SDK设置
if(NOT DEFINED ENV{VK_SDK_PATH})
    message(FATAL_ERROR "VK_SDK_PATH not defined!")
endif()
set(VK_SDK_PATH $ENV{VK_SDK_PATH})
set(VK_SDK_INCLUDE ${VK_SDK_PATH}/Include)
set(VK_SDK_LIB ${VK_SDK_PATH}/Lib/vulkan-1.lib)

add_library(${PROJECT_NAME} STATIC ${_optick_Sources})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${VK_SDK_INCLUDE})
target_link_libraries(optick PUBLIC ${VK_SDK_LIB})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_optick_Sources})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "external")

