project "cppenv"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. "%{cfg.platform}" .. "%{cfg.buildcfg}")
    objdir ("bin-obj/" .. "%{cfg.platform}" .. "%{cfg.buildcfg}")

    files {
        "src/cppenv.h",
        "src/cppenv.cpp",
    }

    filter { "system:windows", "toolset:clang" }
        defines { "_UNICODE", "UNICODE" }

    filter "system:windows"
        toolset "clang"
        files { "src/win32_cppenv.cpp" }

    filter "configurations:*"
        symbols "on"
        buildoptions { "-g -gcodeview" }
        linkoptions { "-Wl,/debug" }

    filter "configurations:Debug"
        runtime "Debug"
        optimize "off"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "action:gmake"
        buildoptions { "-MJ compile_commands.json" }
