add_rules("mode.debug", "mode.release")

add_requires("imgui v1.87-docking", {configs = {glfw_opengl3 = true}})
add_requires("spdlog","glad","glfw") -- glad must before glfw

set_toolchains("clang")
set_languages("c++20")
set_optimize("fastest")
add_defines("ENGINE_PLATFORM_LINUX","ENGINE_ENABLE_ASSERTS")

add_packages("imgui")
add_packages("spdlog","glad","glfw")

target("Engine")
    set_kind("static")
    set_pcheader("Engine/src/pch.hpp")
    add_files("Engine/src/**.cpp")
    add_includedirs("Engine/src")
    
target("Sandbox")
    add_deps("Engine")
    set_kind("binary")
    add_files("Sandbox/src/**.cpp")
    add_includedirs("Engine/src")
