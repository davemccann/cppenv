project "tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/" .. "%{cfg.platform}" .. "%{cfg.buildcfg}")
    objdir ("bin-obj/" .. "%{cfg.platform}" .. "%{cfg.buildcfg}")

    files {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "../os/src",
        "thirdparty/googletest/googletest/include"
    }

    links {
        "os",
        "googletest"
    }

    filter "action:gmake"
        buildoptions { "-MJ compile_commands.json" }
        toolset "clang"

    filter { "system:windows", "toolset:clang" }
        defines { "_UNICODE", "UNICODE" }

    filter "configurations:*"
        symbols "on"
        buildoptions { "-g -gcodeview" }
        linkoptions { "-Wl,/debug" }
        postbuildcommands {
            "py ./../tools/merge_compile_commands.py -p compile_commands.json -d ./.. -o ./../compile_commands.json",
            "{COPYFILE} %[%{wks.location}/.env] %[bin/%{cfg.platform}/%{cfg.buildcfg}]"
        }

    filter "configurations:Debug"
        runtime "Debug"
        defines { "DEBUG" }
        optimize "off"
        buildoptions { "-g -gcodeview" }
        linkoptions { "-Wl,/debug" }


    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    include "thirdparty/googletest"
