
-- Xrandr dependency
if (os.get == "linux" and os.findlib("libXrandr") == nil) then
    os.execute("sudo apt-get install libxrandr-dev")
end

-- visual studio intellisense doesn't work with relative include paths
vs_include_prefix = ""
if string.find(_ACTION, "vs20") then
    vs_include_prefix = "$(SolutionDir)../"
end

src_dir = vs_include_prefix .. "game/"

g_output_dir = os.getcwd() .. "/../bin/"
g_externals_dir = os.getcwd() .. "/external/"

-- A solution contains projects, and defines the available configurations
solution "engine_test"
    location "build"
    startproject "engine_test"
    language "C++"
    
    -- disable cross compiling on linux
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
    configuration "linux"
        toolset "clang"
    configuration {}
    
    -- hidden option to improve optimized code debugging
    configuration "vs*"
        buildoptions
        {
            "/d2Zi+"
        }
    
    -- A project defines one build target
    project "engine_test"
        kind "ConsoleApp"
        targetdir "../bin"
        language "C++"

        forceincludes "platform.h"

        defines { "GLEW_STATIC", "SDL_MAIN_HANDLED", "GLM_FORCE_RADIANS" }
        files { "game/**.h", "game/**.cpp", "game/**.vert", "game/**.frag" }
        includedirs {
            src_dir .. "public/"
        }
        
        -- GLM linking
        dofile "external/glm/include_config.lua"
        
        -- SDL2 linking
        dofile "external/sdl/include_config.lua"
        
        -- Bullet Physics linking
        dofile "external/bulletphysics/include_config.lua"
        
        -- Glew linking
        dofile "external/glew/include_config.lua"
        
        configuration "linux"
            links { "GL", "rt", "m", "pthread", "X11", "Xrandr", "Xi", "Xxf86vm" }
            buildoptions { "-std=c++11" }
        configuration "windows"
            links "opengl32"
