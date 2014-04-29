
local repo = "https://github.com/bulletphysics/bullet3.git"
local dir = path.getbasename(repo)
local current_rev = "9e9b172b234e2ad127e0500bf386a277a9906ade"

function getdir()
    return dir
end

function clone()
    if not os.isdir(getdir() .. "/.git") then
        os.execute("git clone " .. repo .. " " .. getdir())
    end
    os.chdir(dir)
    os.execute("git reset --hard")
    os.execute("git fetch origin master")
    os.execute("git checkout " .. current_rev)
    os.execute("git apply ../bullet_build.patch")
end

function build()
    local build_dir = getdir() .. "/build/"
    os.execute(_PREMAKE_COMMAND .. " vs2013 --without-demos --file=" .. build_dir .. "premake4.lua")
    
    local msbuild = '"' .. os.getenv("VS120COMNTOOLS") .. '../../../MSBuild/12.0/Bin/MSBuild"'
    local build_command = msbuild .. " " .. build_dir .. "vs2013/"
    
    local debug_x32 = "/p:Configuration=Debug /p:Platform=Win32"
    local debug_x64 = "/p:Configuration=Debug /p:Platform=x64"
    local release_x32 = "/p:Configuration=Release /p:Platform=Win32"
    local release_x64 = "/p:Configuration=Release /p:Platform=x64"
    
    os.execute(build_command .. "BulletSoftBody.vcxproj " .. debug_x32)
    os.execute(build_command .. "BulletSoftBody.vcxproj " .. release_x32)
    os.execute(build_command .. "BulletSoftBody.vcxproj " .. debug_x64)
    os.execute(build_command .. "BulletSoftBody.vcxproj " .. release_x64)
    
    os.execute(build_command .. "BulletDynamics.vcxproj " .. debug_x32)
    os.execute(build_command .. "BulletDynamics.vcxproj " .. release_x32)
    os.execute(build_command .. "BulletDynamics.vcxproj " .. debug_x64)
    os.execute(build_command .. "BulletDynamics.vcxproj " .. release_x64)
    
    os.execute(build_command .. "BulletCollision.vcxproj " .. debug_x32)
    os.execute(build_command .. "BulletCollision.vcxproj " .. release_x32)
    os.execute(build_command .. "BulletCollision.vcxproj " .. debug_x64)
    os.execute(build_command .. "BulletCollision.vcxproj " .. release_x64)
    
    os.execute(build_command .. "LinearMath.vcxproj " .. debug_x32)
    os.execute(build_command .. "LinearMath.vcxproj " .. release_x32)
    os.execute(build_command .. "LinearMath.vcxproj " .. debug_x64)
    os.execute(build_command .. "LinearMath.vcxproj " .. release_x64)
end
