
-- Xrandr dependency
if (os.get == "linux" and os.findlib("libXrandr") == nil) then
    os.execute("sudo apt-get install libxrandr-dev")
end

-- A solution contains projects, and defines the available configurations
solution "engine_test"
    location "build"
    startproject "engine_test"
    language "C++"
    configurations { "Debug", "Release" }
    flags { "StaticRuntime", "NoExceptions", "Symbols" }
    vectorextensions "SSE2"
    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_NONSTDC_NO_WARNINGS",
        "_HAS_EXCEPTIONS=0"
    }
    
    configuration "Debug"
        targetdir "build/Debug"
        defines { "DEBUG", "_DEBUG" }
    
    configuration "Release"
        targetdir "build/Release"
        defines { "NDEBUG" }
        optimize "On"
        flags { "LinkTimeOptimization", "MultiProcessorCompile" }

    configuration "linux"
        toolset "clang"

    include "../bullet-2.82-r2704/src/BulletSoftBody"
        configuration "Debug"
            targetdir "build/Debug"
        configuration "Release"
            targetdir "build/Release"
    include "../bullet-2.82-r2704/src/BulletDynamics"
        configuration "Debug"
            targetdir "build/Debug"
        configuration "Release"
            targetdir "build/Release"
    include "../bullet-2.82-r2704/src/BulletCollision"
        configuration "Debug"
            targetdir "build/Debug"
        configuration "Release"
            targetdir "build/Release"
    include "../bullet-2.82-r2704/src/LinearMath"
        configuration "Debug"
            targetdir "build/Debug"
        configuration "Release"
            targetdir "build/Release"
    
    -- A project defines one build target
    project "engine_test"
        kind "ConsoleApp"
        targetdir "build"
        language "C++"
        defines "GLEW_STATIC"
        files { "**.h", "**.cpp", "**.vs", "**.fs" }
        includedirs {
            "../bullet-2.82-r2704/src",
            "../glfw-3.0.4/include",
            "../glew-1.10.0/include",
            "../glm/"
        }
        links { "glew", "glfw", "BulletSoftBody", "BulletDynamics", "BulletCollision", "LinearMath" }
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
    
    project "glfw"
        kind "StaticLib"
        language "C"
        defines "_GLFW_USE_OPENGL"
        includedirs "../glfw-3.0.4/src"
        files {
            "../glfw-3.0.4/src/clipboard.c",
            "../glfw-3.0.4/src/context.c",
            "../glfw-3.0.4/src/gamma.c",
            "../glfw-3.0.4/src/init.c",
            "../glfw-3.0.4/src/input.c",
            "../glfw-3.0.4/src/joystick.c",
            "../glfw-3.0.4/src/monitor.c",
            "../glfw-3.0.4/src/time.c",
            "../glfw-3.0.4/src/window.c"
        }
        
        configuration "windows"
            files {
                "../glfw-3.0.4/src/win32_*",
                "../glfw-3.0.4/src/wgl_*"
            }
            defines {
                "_GLFW_WIN32",
                "_GLFW_WGL"
            }
        
        configuration "macosx"
            files {
                "../glfw-3.0.4/src/cocoa_*",
                "../glfw-3.0.4/src/nsgl_*"
            }
            buildoptions "-fno-common"
            defines {
                "_GLFW_NSGL",
                "_GLFW_COCOA"
            }
            linkoptions {
                "-framework OpenGL",
                "-framework Cocoa",
                "-framework IOKit"
            }
        
        configuration "linux"
            files {
                "../glfw-3.0.4/src/x11_*",
                "../glfw-3.0.4/src/glx_*"
            }
            defines {
                "_GLFW_X11",
                "_GLFW_GLX",
                "_GLFW_USE_LINUX_JOYSTICKS",
                "_GLFW_HAS_XRANDR",
                "_GLFW_HAS_PTHREAD",
                "_GLFW_HAS_SCHED_YIELD",
                "_GLFW_HAS_GLXGETPROCADDRESS"
            }
