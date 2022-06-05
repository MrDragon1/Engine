add_rules("mode.debug", "mode.release")

set_toolchains("clang")
set_languages("c++20")
set_optimize("fastest")
add_defines("ENGINE_PLATFORM_LINUX")


target("Engine")
    set_kind("static")
    add_files("Engine/src/**.cpp")
    add_includedirs("Engine/src")
    
target("Sandbox")
    set_kind("binary")
    add_deps("Engine")
    add_files("Sandbox/src/**.cpp")
    add_includedirs("Engine/src")