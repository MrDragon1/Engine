add_rules("mode.debug", "mode.release")

add_requires("imgui v1.87-docking", {configs = {glfw_opengl3 = true}})
add_requires("spdlog","glad","glfw","glm","entt","yaml-cpp","box2d") -- glad must before glfw


set_languages("cxx20")
set_optimize("fastest")
add_defines("ET_PLATFORM_LINUX","ET_ENABLE_ASSERTS")

add_packages("imgui")
add_packages("spdlog","glad","glfw","glm","entt","yaml-cpp","box2d")

target("Editor")
    set_default(true)
    set_toolchains("clang")
    set_rundir("$(projectdir)/Editor")
    add_deps("Engine")
    set_kind("binary")
    add_files("Editor/src/**.cpp")
    add_includedirs("Ethereal/src","Ethereal/vendor")


target("Engine")
    set_toolchains("clang")
    set_kind("static")
    set_pcxxheader("Ethereal/src/pch.hpp")

    add_files("Ethereal/src/**.cpp","Ethereal/vendor/**.cpp")
    add_includedirs("Ethereal/src","Ethereal/vendor")
    add_syslinks("comdlg32")