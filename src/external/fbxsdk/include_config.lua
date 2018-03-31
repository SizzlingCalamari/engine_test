
local lib_dir = ""
local lib_name = ""
local path_sep = ""
local copy_cmd = ""
if (os.get() == "windows") then
    lib_dir = (g_externals_dir .. "fbxsdk/fbxsdk/lib/vs2013/")
    lib_name = "libfbxsdk.dll"
    path_sep = "\\"
    copy_cmd = "copy "
elseif (os.get() == "linux") then
    lib_dir = (g_externals_dir .. "fbxsdk/fbxsdk/lib/gcc4/")
    lib_name = "libfbxsdk.so"
    path_sep = "/"
    copy_cmd = "cp -f "
elseif (os.get() == "macosx") then
    lib_dir = (g_externals_dir .. "fbxsdk/fbxsdk/lib/clang/")
    lib_name = "libfbxsdk.dylib"
    path_sep = "/"
    copy_cmd = "cp -f "
end

includedirs (g_externals_dir .. "fbxsdk/fbxsdk/include")

configuration { "x32", "Debug" }
    libdirs (lib_dir .. "x86/debug")
configuration { "x32", "Release" }
    libdirs (lib_dir .. "x86/release")
configuration { "x64", "Debug" }
    libdirs (lib_dir .. "x64/debug")
configuration { "x64", "Release" }
    libdirs (lib_dir .. "x64/release")
configuration {}

configuration { "windows" }
    links { "libfbxsdk" }
configuration { "linux or macosx" }
    links { "fbxsdk" }
configuration {}

configuration { "x32", "Debug" }
    local fullLibPath = (lib_dir .. "x86/debug/" .. lib_name)
    postbuildcommands { path.translate(copy_cmd .. fullLibPath .. " " .. g_output_dir, path_sep) }
configuration { "x32", "Release" }
    local fullLibPath = (lib_dir .. "x86/release/" .. lib_name)
    postbuildcommands { path.translate(copy_cmd .. fullLibPath .. " " .. g_output_dir, path_sep) }
configuration { "x64", "Debug" }
    local fullLibPath = (lib_dir .. "x64/debug/" .. lib_name)
    postbuildcommands { path.translate(copy_cmd .. fullLibPath .. " " .. g_output_dir, path_sep) }
configuration { "x64", "Release" }
    local fullLibPath = (lib_dir .. "x64/release/" .. lib_name)
    postbuildcommands { path.translate(copy_cmd .. fullLibPath .. " " .. g_output_dir, path_sep) }
configuration {}
