add_rules("mode.debug", "mode.release")
package("tinyobjloader")
    set_homepage("https://github.com/tinyobjloader/tinyobjloader")
    set_description("Tiny but powerful single file wavefront obj loader")
    set_license("MIT")

    add_urls("https://github.com/tinyobjloader/tinyobjloader.git")

    add_configs("double", {description = "Use double precision floating numbers.", default = false, type = "boolean"})
    if is_plat("windows") then
        add_configs("shared", {description = "Build shared library.", default = false, type = "boolean", readonly = true})
    end

    on_load(function (package)
        if package:config("double") then
            package:add("defines", "TINYOBJLOADER_USE_DOUBLE")
        end
    end)

    on_install("macosx", "linux", "windows", "mingw", "android", "iphoneos", function (package)
        io.writefile("xmake.lua", string.format([[
            add_rules("mode.debug", "mode.release")
            add_rules("utils.install.cmake_importfiles")
            target("tinyobjloader")
                set_kind("$(kind)")
                %s
                add_files("tiny_obj_loader.cc")
                add_headerfiles("tiny_obj_loader.h")
        ]], package:config("double") and "add_defines(\"TINYOBJLOADER_USE_DOUBLE\")" or ""))
        import("package.tools.xmake").install(package)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <vector>
            void test() {
                tinyobj::attrib_t attrib;
                std::vector<tinyobj::shape_t> shapes;
                std::vector<tinyobj::material_t> materials;
            }
        ]]}, {configs = {languages = "c++11"}, includes = "tiny_obj_loader.h"}))
    end)
package_end()


add_requires("imgui v1.87-docking", {configs = {glfw_opengl3 = true}})
add_requires("spdlog","glad","glfw","glm","entt","yaml-cpp","box2d","tinyobjloader") -- glad must before glfw


set_languages("cxx20")
set_optimize("fastest")
add_defines("ET_PLATFORM_LINUX","ET_ENABLE_ASSERTS")
add_defines("ENGINE_ROOT_DIR=$(projectdir)")

add_packages("imgui")
add_packages("spdlog","glad","glfw","glm","entt","yaml-cpp","box2d","tinyobjloader")

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