
local repo = "https://github.com/bulletphysics/bullet3.git"
local dir = path.getbasename(repo)
local current_rev = "6b2cae1b1d63056ef48c64b39c8db6027e897663"

function getdir()
    return dir
end

function clone()
    if os.isdir(getdir() .. "/.git") then
        return
    end
    
    os.execute("git clone --single-branch --branch master " .. repo .. " " .. getdir())
    
    local success, msg, errno = os.chdir(dir)
    if not success then
        error(msg)
    end
    os.execute("git checkout " .. current_rev)
    os.execute("git clean -ffdx .")
end

function buildLinux()
    local build_dir = getdir() .. "/build3/"
    os.execute(_PREMAKE_COMMAND .. " gmake --file=premake4.lua")

    local build_command = "cd bullet3/build3/gmake; make -j4 "
    os.execute(build_command .. "config=debug_x64")
    os.execute(build_command .. "config=release_x64")
end

function buildWindows()
    os.execute(_PREMAKE_COMMAND .. " vs2017 --file=premake4.lua")

    os.chdir(getdir() .. "/build3/")
    
    local build_command = os.getenv("MSBUILD_PATH") .. " vs2017/"
    print(build_command)
    
    local debug_x64 = "/p:Configuration=Debug /p:Platform=x64 /p:PlatformToolset=v141"
    local release_x64 = "/p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v141"
    os.execute(build_command .. "BulletSoftBody.vcxproj " .. debug_x64)
    os.execute(build_command .. "BulletSoftBody.vcxproj " .. release_x64)

    os.execute(build_command .. "BulletDynamics.vcxproj " .. debug_x64)
    os.execute(build_command .. "BulletDynamics.vcxproj " .. release_x64)

    os.execute(build_command .. "BulletCollision.vcxproj " .. debug_x64)
    os.execute(build_command .. "BulletCollision.vcxproj " .. release_x64)

    os.execute(build_command .. "LinearMath.vcxproj " .. debug_x64)
    os.execute(build_command .. "LinearMath.vcxproj " .. release_x64)
end

