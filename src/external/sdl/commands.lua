
local repo = "https://github.com/spurious/SDL-mirror.git"
local branch = "release-2.0.4"
local dir = "SDL"

function getdir()
    return dir
end

function clone()
    if not os.isdir(getdir() .. "/.git") then
        os.execute("git clone --depth 1 --branch " .. branch .. " " .. repo .. " " .. getdir())
    end
    local success, msg, errno = os.chdir(dir)
    if not success then
        error(msg)
    end
    os.execute("git checkout -- .")
    os.execute("git clean -ffdx .")
end

function buildLinux()
    if (not os.isfile("Makefile")) then
        os.execute("cd " .. dir .. "; ./autogen.sh; ./configure; make -j4")
    else
        os.execute("cd " .. dir .. "; make -j4")
    end
end

function buildWindows()
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
