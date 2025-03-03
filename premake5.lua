workspace "MyProject"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "tests"

    -- Force Clang as the compiler
    toolset "clang"
    
    -- Define the output directory format
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Include subprojects
    include "cppenv"
    include "tests"
