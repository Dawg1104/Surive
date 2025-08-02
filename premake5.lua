workspace "Survive"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Survive"

-- GLFW Project (static lib)
project "glfw"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("bin/%{cfg.buildcfg}")
    objdir ("bin-int/%{cfg.buildcfg}/%{prj.name}")

    files {
        "external/glfw/include/**.h",
        "external/glfw/src/**.c"
    }

    includedirs {
        "external/glfw/include"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

-- Survive Game Project
project "Survive"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/%{cfg.buildcfg}")
    objdir ("bin-int/%{cfg.buildcfg}/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp",
        "external/imgui/*.cpp",
        "external/imgui/backends/imgui_impl_glfw.cpp",
        "external/imgui/backends/imgui_impl_opengl3.cpp",
        "external/glad/src/glad.c"
    }

    includedirs {
        "src",
        "external/glfw/include",
        "external/glad/include",
        "external/stb",
        "external/imgui",
        "external/imgui/backends"
    }

    links {
        "glfw",
        "opengl32"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "WIN32", "_WINDOWS" }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"