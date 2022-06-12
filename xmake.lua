add_rules("mode.debug", "mode.release")

add_requires("imgui v1.87-docking", {configs = {glfw_opengl3 = true}})
add_requires("spdlog","glad","glfw","glm") -- glad must before glfw

if (is_plat("linux")) then
   set_toolchains("clang")
end

set_languages("c++20")
set_optimize("fastest")
add_defines("ENGINE_PLATFORM_LINUX","ENGINE_ENABLE_ASSERTS")

add_packages("imgui")
add_packages("spdlog","glad","glfw","glm")

target("Engine")
    set_kind("static")
    set_pcxxheader("Engine/src/pch.hpp")
    add_files("Engine/src/**.cpp","Engine/vendor/**.cpp")
    add_includedirs("Engine/src","Engine/vendor")

    
target("Sandbox")
    add_deps("Engine")
    set_kind("binary")
    add_files("Sandbox/src/**.cpp")
    add_includedirs("Engine/src")

target("Editor")
    add_deps("Engine")
    set_kind("binary")
    add_files("Editor/src/**.cpp")
    add_includedirs("Engine/src")

