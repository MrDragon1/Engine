add_rules("mode.debug", "mode.release")
add_requires("spdlog")

set_toolchains("clang")
set_languages("c99","c++20")
set_optimize("fastest")
add_defines("ENGINE_PLATFORM_LINUX")

add_packages("spdlog")

target("Engine")
    set_kind("static")
    add_files("Engine/src/**.cpp")
    add_includedirs("Engine/src")
    
    
target("Sandbox")
    add_deps("Engine")
    set_kind("binary")
    add_files("Sandbox/src/**.cpp")
    add_includedirs("Engine/src")
