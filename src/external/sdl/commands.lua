
local repo = "https://github.com/spurious/SDL-mirror.git"
local branch = "release-2.0.8"
local dir = "SDL"

function getdir()
    return dir
end

function clone()
    if os.isdir(getdir() .. "/.git") then
        return
    end

    os.execute("git clone --depth 1 --branch " .. branch .. " " .. repo .. " " .. getdir())

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
    local build_command = os.getenv("MSBUILD_PATH") .. " " .. build_dir
    
    local debug_x64 = "/p:Configuration=Debug /p:Platform=x64 /p:PlatformToolset=v141"
    local release_x64 = "/p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v141"
    
    os.execute(build_command .. "SDL.vcxproj " .. debug_x64)
    os.execute(build_command .. "SDL.vcxproj " .. release_x64)
end
