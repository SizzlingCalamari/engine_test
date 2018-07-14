
    solution "0_Bullet3Solution"

    configurations {"Release", "Debug"}
    
    flags { "MultiProcessorCompile" }
    
    configuration "vs*"
        buildoptions "/wd4244 /wd4267"
    configuration "Release"
        flags { "Optimize", "EnableSSE2", "NoMinimalRebuild", "FloatFast" }
    configuration "Debug"
        defines {"_DEBUG=1"}
        flags { "Symbols", "NoMinimalRebuild", "NoEditAndContinue" ,"FloatFast" }
    configuration {}
    
    platforms "x64"

    configuration {"x64", "Debug"}
        targetsuffix ("_vs2017_x64_debug")
    configuration {"x64", "Release"}
        targetsuffix ("_vs2017_x64_release" )
    configuration {}

    targetdir "bullet3/lib"
    location("bullet3/build3/vs2017")

    language "C++"

    include "bullet3/src/BulletInverseDynamics"
    include "bullet3/src/BulletSoftBody"
    include "bullet3/src/BulletDynamics"
    include "bullet3/src/BulletCollision"
    include "bullet3/src/LinearMath"

