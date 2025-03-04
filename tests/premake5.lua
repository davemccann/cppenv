project "tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/" .. "%{cfg.platform}" .. "%{cfg.buildcfg}")
    objdir ("bin-obj/" .. "%{cfg.platform}" .. "%{cfg.buildcfg}")
    debugdir ("bin/" .. "%{cfg.platform}" .. "%{cfg.buildcfg}")

    files {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "../cppenv/src",
        "thirdparty/googletest/googletest/include"
    }

    links {
        "cppenv",
        "googletest"
    }

    filter "action:gmake"
        buildoptions { "-MJ compile_commands.json" }
        toolset "clang"

    filter { "system:windows", "toolset:clang" }
        defines { "_UNICODE", "UNICODE" }
        buildoptions { "-g -gcodeview" }
        linkoptions { "-Wl,/debug" }
        postbuildcommands {
            "py ./../tools/merge_compile_commands.py -p compile_commands.json -d ./.. -o ./../compile_commands.json",
        }

    filter "configurations:*"
        symbols "on"
        postbuildcommands { "{COPYFILE} %[%{wks.location}/.env] %[bin/%{cfg.platform}/%{cfg.buildcfg}]" }

    filter "configurations:Debug"
        runtime "Debug"
        defines { "DEBUG" }
        optimize "off"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    include "thirdparty/googletest"
