
local dir = "fbxsdk"

function getdir()
    return dir
end

function clone()
    if os.get() == "macosx" then
        os.execute("rm -rf fbxsdk")
        os.execute("wget http://images.autodesk.com/adsk/files/fbx20151_fbxsdk_clang_mac.pkg.tgz")
        os.execute("tar xzf fbx20151_fbxsdk_clang_mac.pkg.tgz")
        os.execute("tar xzf fbx20151_fbxsdk_clang_macos.pkg/Contents/Archive.pax.gz -C .")
        os.execute("mv 'Applications/Autodesk/FBX SDK/2015.1' fbxsdk")
        -- move fbxsdk/lib/clang/* to fbxsdk/lib/clang/x64/*
        os.execute("mv fbxsdk/lib/clang fbxsdk/lib/tmp")
        os.execute("mkdir fbxsdk/lib/clang")
        os.execute("mv fbxsdk/lib/tmp fbxsdk/lib/clang/x64")
        -- cleanup
        os.execute("rm fbx20151_fbxsdk_clang_mac.pkg.tgz")
        os.execute("rm -rf fbx20151_fbxsdk_clang_macos.pkg")
        os.execute("rm -rf Applications")
    end
end

function buildLinux()
end

function buildWindows()
end

