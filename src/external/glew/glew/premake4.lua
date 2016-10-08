
solution "glew"
    location ("build/" .. _ACTION)
    startproject "glew"
    language "C"

    if os.is("linux") then
        if os.is64bit() then
            platforms "x64"
        else
            platforms "x32"
        end
    else
        platforms { "x32", "x64" }
    end

    flags { "StaticRuntime", "NoExceptions", "Symbols" }
    vectorextensions "SSE2"

    configurations { "Debug", "Release" }
    configuration "Debug"
        defines { "DEBUG", "_DEBUG" }
    configuration "Release"
        defines { "NDEBUG" }
        optimize "Full"
        flags { "LinkTimeOptimization", "MultiProcessorCompile" }
    configuration {}

    configuration "windows"
        defines "WIN32"
    configuration {}

    -- hidden option to improve optimized code debugging
    configuration "vs*"
        buildoptions { "/d2Zi+" }
    
    project "glew"
        targetdir "lib"
        kind "StaticLib"
        defines "GLEW_STATIC"
        includedirs { "include" }
        files {
            "src/glew.c",
            "src/glewinfo.c"
        }
        configuration {"x32", "Debug"}
            targetsuffix ("_" .. _ACTION .. "_x86_debug" )
        configuration {"x32", "Release"}
            targetsuffix ("_" .. _ACTION .. "_x86_release" )
        configuration {"x64", "Debug"}
            targetsuffix ("_" .. _ACTION .. "_x64_debug")
        configuration {"x64", "Release"}
            targetsuffix ("_" .. _ACTION .. "_x64_release" )
