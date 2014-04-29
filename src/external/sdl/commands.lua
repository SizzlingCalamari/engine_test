
local repo = "http://hg.libsdl.org/SDL"
local dir = path.getbasename(repo)

function getdir()
    return dir
end

function clone()
    if not os.isdir(getdir() .. "/.hg") then
        os.execute("hg clone " .. repo .. " " .. getdir())
    end
end

function build()
    local build_dir = getdir() .. "/VisualC/SDL/"
    local msbuild = '"' .. os.getenv("VS120COMNTOOLS") .. '../../../MSBuild/12.0/Bin/MSBuild"'
    local build_command = msbuild .. " " .. build_dir
    
    local debug_x32 = "/p:Configuration=Debug /p:Platform=Win32"
    local debug_x64 = "/p:Configuration=Debug /p:Platform=x64"
    local release_x32 = "/p:Configuration=Release /p:Platform=Win32"
    local release_x64 = "/p:Configuration=Release /p:Platform=x64"
    
    print(build_command .. "SDL_VS2013.vcxproj " .. debug_x32)
    
    os.execute(build_command .. "SDL_VS2013.vcxproj " .. debug_x32)
    os.execute(build_command .. "SDL_VS2013.vcxproj " .. release_x32)
    os.execute(build_command .. "SDL_VS2013.vcxproj " .. debug_x64)
    os.execute(build_command .. "SDL_VS2013.vcxproj " .. release_x64)
end
