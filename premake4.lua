
-- Xrandr dependency
if (os.get == "linux" and os.findlib("libXrandr") == nil) then
    os.execute("sudo apt-get install libxrandr-dev")
end

sdl_dir = "../sdl-dev/"
bullet_dir = "../bullet-2.82-r2704/"
glew_dir = "../glew-1.10.0/"
glm_dir = "../glm/"
soil_dir = "../soil-july07-2008/"

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
        optimize "On"
        flags { "LinkTimeOptimization", "MultiProcessorCompile" }
    
    -- os specific configurations
    configuration "windows"
        defines "WIN32"
    configuration "linux"
        toolset "clang"
    
    -- A project defines one build target
    project "engine_test"
        kind "ConsoleApp"
        targetdir "build"
        language "C++"

        pchheader "platform.h"
        pchsource "platform.pch"

        defines { "GLEW_STATIC", "SDL_MAIN_HANDLED" }
        files { "**.h", "**.cpp", "**.vs", "**.fs" }
        includedirs {
            "src",
            sdl_dir .. "include",
            bullet_dir .. "src",
            glew_dir .. "include",
            glm_dir,
            soil_dir .. "src"
        }
        
        -- SDL2 linking
        libdirs (sdl_dir .. "VisualC")
        configuration { "windows", "x32", "Debug" }
            links { "/SDL/Win32/Debug/SDL2", "/SDLmain/Win32/Debug/SDL2main" }
            copylocal "/SDL/Win32/Debug/SDL2"
        configuration { "windows", "x32", "Release" }
            links { "/SDL/Win32/Release/SDL2", "/SDLmain/Win32/Release/SDL2main" }
            copylocal "/SDL/Win32/Release/SDL2"
        configuration { "windows", "x64", "Debug" }
            links { "/SDL/x64/Debug/SDL2", "/SDLmain/x64/Debug/SDL2main" }
            copylocal "/SDL/x64/Debug/SDL2"
        configuration { "windows", "x64", "Release" }
            links { "/SDL/x64/Release/SDL2", "/SDLmain/x64/Release/SDL2main" }
            copylocal "/SDL/x64/Release/SDL2"
        configuration { "linux" }
            links { "SDL2" }
        -- TODO: osx sdl linking
        configuration {}
        
        -- Bullet Physics linking
        libdirs (bullet_dir .. "lib")
        configuration { "x32", "Debug" }
            suffix = _ACTION .. "_debug"
            links {
                "BulletSoftBody_" .. suffix,
                "BulletDynamics_" .. suffix,
                "BulletCollision_" .. suffix,
                "LinearMath_" .. suffix
            }
        configuration { "x32", "Release" }
            suffix = _ACTION
            links {
                "BulletSoftBody_" .. suffix,
                "BulletDynamics_" .. suffix,
                "BulletCollision_" .. suffix,
                "LinearMath_" .. suffix
            }
        configuration { "x64", "Debug" }
            suffix = _ACTION .. "_x64_debug"
            links {
                "BulletSoftBody_" .. suffix,
                "BulletDynamics_" .. suffix,
                "BulletCollision_" .. suffix,
                "LinearMath_" .. suffix
            }
        configuration { "x64", "Release" }
            suffix = _ACTION .. "_x64_release"
            links {
                "BulletSoftBody_" .. suffix,
                "BulletDynamics_" .. suffix,
                "BulletCollision_" .. suffix,
                "LinearMath_" .. suffix
            }
        configuration {}
        
        links { "glew", "soil" }
        configuration "linux"
            links { "GL", "rt", "m", "pthread", "X11", "Xrandr", "Xi", "Xxf86vm" }
            buildoptions { "-std=c++11" }
        configuration "windows"
            links "opengl32"

    project "glew"
        kind "StaticLib"
        language "C"
        defines "GLEW_STATIC"
        includedirs (glew_dir .. "include")
        files {
            glew_dir .. "src/glew.c",
            glew_dir .. "src/glewinfo.c"
        }
    
    project "soil"
        kind "StaticLib"
        language "C"
        includedirs (soil_dir .. "src")
        files {
            soil_dir .. "src/image_helper.c",
            soil_dir .. "src/stb_image_aug.c",
            soil_dir .. "src/image_DXT.c",
            soil_dir .. "src/SOIL.c"
        }

