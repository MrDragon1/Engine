add_rules("mode.debug", "mode.release")

add_requires("imgui v1.87-docking", {configs = {glfw_opengl3 = true}})
add_requires("spdlog","glad","glfw","glm","entt","yaml-cpp","box2d") -- glad must before glfw


set_languages("cxx20")
set_optimize("fastest")
add_defines("ENGINE_PLATFORM_LINUX","ENGINE_ENABLE_ASSERTS")

add_packages("imgui")
add_packages("spdlog","glad","glfw","glm","entt","yaml-cpp","box2d")

target("Editor")
    set_default(true)
    set_toolchains("clang")
    set_rundir("$(projectdir)/Editor")
    add_deps("Engine")
    set_kind("binary")
    add_files("Editor/src/**.cpp")
    add_includedirs("Engine/src","Engine/vendor")


target("Engine")
    set_toolchains("clang")
    set_kind("static")
    set_pcxxheader("Engine/src/pch.hpp")

    add_files("Engine/src/**.cpp","Engine/vendor/**.cpp")
    add_includedirs("Engine/src","Engine/vendor")
    add_syslinks("comdlg32")

target("Sandbox")
    set_default(false)
    set_toolchains("clang")
    add_deps("Engine")
    set_kind("binary")
    add_files("Sandbox/src/**.cpp")
    add_includedirs("Engine/src")



