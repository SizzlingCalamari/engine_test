
local repo = "https://github.com/nigels-com/glew.git"
local dir = path.getbasename(repo)

function getdir()
    return dir
end

function clone()
end

function buildLinux()
    os.execute(_PREMAKE_COMMAND .. " gmake --file=" .. getdir() .. "/premake4.lua")

    local build_command = "cd glew/build/gmake; make -j4 "
    os.execute(build_command .. "config=debug_x64")
    os.execute(build_command .. "config=release_x64")
end

function buildWindows()
    os.execute(_PREMAKE_COMMAND .. " vs2017 --file=" .. getdir() .. "/premake4.lua")
    
    local build_command = os.getenv("MSBUILD_PATH") .. " " .. getdir() .. "/build/vs2017/"
    
    local debug_x64 = "/p:Configuration=Debug /p:Platform=x64 /p:PlatformToolset=v141"
    local release_x64 = "/p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v141"

    os.execute(build_command .. "glew.vcxproj " .. debug_x64)
    os.execute(build_command .. "glew.vcxproj " .. release_x64)
end


