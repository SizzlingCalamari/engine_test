
local subdir_name = "vs2013/"
if (os.get() == "linux") then
    subdir_name = "gcc4/"
end

local lib_dir = g_externals_dir .. "fbxsdk/fbxsdk/lib/" .. subdir_name
local copy_cmd = "copy " .. lib_dir

includedirs (g_externals_dir .. "fbxsdk/fbxsdk/include")
libdirs ()

local lib_path = ""
configuration { "x32", "Debug" }
    libdirs ("fbxsdk/lib/" .. subdir_name .. "/x86/debug")
configuration { "x32", "Release" }
    libdirs ("fbxsdk/lib/" .. subdir_name .. "/x86/release")
configuration { "x64", "Debug" }
    libdirs ("fbxsdk/lib/" .. subdir_name .. "/x64/debug")
configuration { "x64", "Release" }
    libdirs ("fbxsdk/lib/" .. subdir_name .. "/x64/release")
configuration {}

links { "fbxsdk" }

configuration { "windows" }
    postbuildcommands { path.translate(copy_cmd .. lib_path .. ".dll " .. g_output_dir) }
configuration {}

