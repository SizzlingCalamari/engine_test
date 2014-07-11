
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
    os.execute(build_command .. "config=debug_x32")
    os.execute(build_command .. "config=debug_x64")
    os.execute(build_command .. "config=release_x32")
    os.execute(build_command .. "config=release_x64")
end

function buildWindows()
    os.execute(_PREMAKE_COMMAND .. " vs2013 --file=" .. getdir() .. "/premake4.lua")
    
    local msbuild = '"' .. os.getenv("VS120COMNTOOLS") .. '../../../MSBuild/12.0/Bin/MSBuild"'
    local build_command = msbuild .. " " .. getdir() .. "/build/vs2013/"
    
    local debug_x32 = "/p:Configuration=Debug /p:Platform=Win32"
    local debug_x64 = "/p:Configuration=Debug /p:Platform=x64"
    local release_x32 = "/p:Configuration=Release /p:Platform=Win32"
    local release_x64 = "/p:Configuration=Release /p:Platform=x64"
    
    os.execute(build_command .. "glew.vcxproj " .. debug_x32)
    os.execute(build_command .. "glew.vcxproj " .. release_x32)
    os.execute(build_command .. "glew.vcxproj " .. debug_x64)
    os.execute(build_command .. "glew.vcxproj " .. release_x64)
end


