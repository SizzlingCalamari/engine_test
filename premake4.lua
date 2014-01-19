
-- Xrandr dependency
if (os.get == "linux" and os.findlib("libXrandr") == nil) then
    os.execute("sudo apt-get install libxrandr-dev")
end

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
        defines { "GLEW_STATIC", "SDL_MAIN_HANDLED" }
        files { "**.h", "**.cpp", "**.vs", "**.fs" }
        includedirs {
            "../sdl-dev/include",
            "../bullet-2.82-r2704/src",
            "../glfw-3.0.4/include",
            "../glew-1.10.0/include",
            "../glm/",
            "../Simple OpenGL Image Library/src"
        }
        
        -- SDL2 linking
        libdirs "../sdl-dev/VisualC"
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
        -- TODO: linux, osx sdl linking
        configuration {}
        
        -- Bullet Physics linking
        libdirs "../bullet-2.82-r2704/lib"
        configuration { "windows", "x32", "Debug" }
            links {
                "BulletSoftBody_vs2013_debug",
                "BulletDynamics_vs2013_debug",
                "BulletCollision_vs2013_debug",
                "LinearMath_vs2013_debug"
            }
        configuration { "windows", "x32", "Release" }
            links {
                "BulletSoftBody_vs2013",
                "BulletDynamics_vs2013",
                "BulletCollision_vs2013",
                "LinearMath_vs2013"
            }
        configuration { "windows", "x64", "Debug" }
            links {
                "BulletSoftBody_vs2013_x64_debug",
                "BulletDynamics_vs2013_x64_debug",
                "BulletCollision_vs2013_x64_debug",
                "LinearMath_vs2013_x64_debug"
            }
        configuration { "windows", "x64", "Release" }
            links {
                "BulletSoftBody_vs2013_x64_release",
                "BulletDynamics_vs2013_x64_release",
                "BulletCollision_vs2013_x64_release",
                "LinearMath_vs2013_x64_release"
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
        includedirs "../glew-1.10.0/include"
        files {
            "../glew-1.10.0/src/glew.c",
            "../glew-1.10.0/src/glewinfo.c"
        }
    
    project "soil"
        kind "StaticLib"
        language "C"
        includedirs "../Simple OpenGL Image Library/src"
        files {
            "../Simple OpenGL Image Library/src/image_helper.c",
            "../Simple OpenGL Image Library/src/stb_image_aug.c",
            "../Simple OpenGL Image Library/src/image_DXT.c",
            "../Simple OpenGL Image Library/src/SOIL.c"
        }
