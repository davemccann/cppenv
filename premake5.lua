workspace "CppEnv"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "tests"

    filter "action:gmake"
        buildoptions { "-MJ compile_commands.json" }
        toolset "clang"

    -- Define the output directory format
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Include subprojects
    include "cppenv"
    include "tests"
