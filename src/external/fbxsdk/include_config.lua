
local lib_dir = g_externals_dir .. "fbxsdk/fbxsdk/lib/vs2013/"

local copy_cmd = "copy " .. lib_dir
local copy_dest = g_output_dir .. "libfbxsdk.dll"
local full_cmd = ""

includedirs (g_externals_dir .. "fbxsdk/fbxsdk/include")
libdirs ("fbxsdk/lib/vs2013")

configuration { "windows", "x32", "Debug" }
    local lib_path = "/x86/debug/libfbxsdk"
    links { lib_path }
    postbuildcommands { path.translate(copy_cmd .. lib_path .. ".dll " .. g_output_dir) }
configuration { "windows", "x32", "Release" }
    local lib_path = "/x86/release/libfbxsdk"
    links { lib_path }
    postbuildcommands { path.translate(copy_cmd .. lib_path .. ".dll " .. g_output_dir) }
configuration { "windows", "x64", "Debug" }
    local lib_path = "/x64/debug/libfbxsdk"
    links { lib_path }
    postbuildcommands { path.translate(copy_cmd .. lib_path .. ".dll " .. g_output_dir) }
configuration { "windows", "x64", "Release" }
    local lib_path = "/x64/release/libfbxsdk"
    links { lib_path }
    full_cmd = 
    postbuildcommands { path.translate(copy_cmd .. lib_path .. ".dll " .. g_output_dir) }
configuration { "linux" }
    links { "SDL2" }
-- TODO: osx linking
configuration {}
