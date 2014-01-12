-- A solution contains projects, and defines the available configurations
solution "engine_test"
    configurations { "Debug", "Release" }
    flags { "StaticRuntime" }
 
    -- A project defines one build target
    project "engine_test"
        kind "ConsoleApp"
        language "C++"
        location "build"
        files { "**.h", "**.cpp" }
        includedirs {
            "../bullet-2.82-r2704/src"
        }
        libdirs {
            "../bullet-2.82-r2704/lib"
        }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }
        links {
            "BulletCollision_vs2013_debug",
            "BulletDynamics_vs2013_debug",
            "LinearMath_vs2013_debug",
        }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
        links {
            "BulletCollision_vs2013",
            "BulletDynamics_vs2013",
            "LinearMath_vs2013"
        }