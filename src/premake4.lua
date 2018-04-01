
-- Xi dependency
if (os.get() == "linux" and os.findlib("libxi") == nil) then
    os.execute("sudo apt-get install libxi-dev")
end

-- visual studio intellisense doesn't work with relative include paths
vs_include_prefix = ""
if string.find(_ACTION, "vs20") then
    vs_include_prefix = "$(SolutionDir)../"
else
    vs_include_prefix = os.getcwd() .. "/"
end

src_dir = vs_include_prefix .. "game/"

g_output_dir = os.getcwd() .. "/../bin/"
g_externals_dir = vs_include_prefix .. "/external/"

local pathsep = "/"
if os.is("windows") then
    pathsep = "\\"
end

-- A solution contains projects, and defines the available configurations
solution "engine_test"
    location "build"
    startproject "engine_test"
    language "C++"
    
    -- disable cross compiling on linux
    if os.is("linux") or os.is("macosx") then
        if os.is64bit() then
            platforms "x64"
        else
            platforms "x32"
        end
    else
        platforms { "x32", "x64" }
    end
    
    flags { "StaticRuntime", "Symbols" }
    if exceptionhandling ~= nil then
        exceptionhandling "Off"
    else
        flags { "NoExceptions" }
    end
    vectorextensions "SSE2"
    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_NONSTDC_NO_WARNINGS",
        "_HAS_EXCEPTIONS=0"
    }
    
    -- build configurations
    configurations { "Debug", "Release" }
    configuration "Debug"
        targetdir "build/Debug"
        defines { "DEBUG", "_DEBUG" }
    configuration "Release"
        targetdir "build/Release"
        defines { "NDEBUG" }
        optimize "Full"
        flags { "LinkTimeOptimization", "MultiProcessorCompile" }
    configuration {}
    
    -- os specific configurations
    configuration "windows"
        defines "WIN32"
    configuration {}
    
    -- hidden option to improve optimized code debugging
    configuration "vs*"
        buildoptions
        {
            "/d2Zi+"
        }
    
    -- Remove sse2 specification on windows 64 bit compile to 
    -- get rid of unknown option /arch:SSE2 warning.
    -- Note: SSE2 is still generated
    configuration { "vs*", "x64" }
        vectorextensions "Default"
    
    -- A project defines one build target
    project "engine_test"
        kind "ConsoleApp"
        targetdir "../bin"
        language "C++"

        configuration { "windows" }
            forceincludes "platform.h"
        configuration { "linux or macosx" }
            forceincludes "game/public/platform.h"
        configuration {}

        defines { "GLEW_STATIC", "GLEW_NO_GLU", "SDL_MAIN_HANDLED", "GLM_FORCE_RADIANS" }
        files { "game/**.h", "game/**.cpp", "game/**.vert", "game/**.frag", "game/**.glsl" }
        includedirs {
            src_dir .. "public/"
        }
        print (src_dir.."public/")
        
        -- GLM linking
        dofile "external/glm/include_config.lua"
        
        -- SDL2 linking
        dofile "external/sdl/include_config.lua"
        
        -- Bullet Physics linking
        dofile "external/bulletphysics/include_config.lua"
        
        -- Glew linking
        dofile "external/glew/include_config.lua"
        
        -- fbxsdk linking
        dofile "external/fbxsdk/include_config.lua"

        configuration "linux"
            links { "GL", "rt", "m", "pthread", "X11", "Xi" }
            buildoptions { "-std=c++11", "-msse3" }
        configuration "macosx"
            links { "OpenGL.framework", "m", "pthread" }
            buildoptions { "-std=c++11" }
        configuration "windows"
            links "opengl32"
        configuration {}
        
        local shader_src = path.translate(os.getcwd() .. "/game/shaders/*", pathsep)
        local shader_dest = path.translate(os.getcwd() .. "/../shaders/", pathsep)
        configuration { "windows" }
            postbuildcommands {
                "copy /Y \"" .. shader_src .. "\" \"" .. shader_dest .. "\""
            }
        configuration { "linux or macosx" }
            postbuildcommands {
                "cp -r " .. shader_src .. " \"" .. shader_dest .. "\""
            }
        configuration {}

