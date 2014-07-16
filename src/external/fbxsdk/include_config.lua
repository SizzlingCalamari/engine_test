
local lib_dir = ""
local lib_name = ""
if (os.get() == "windows") then
    lib_dir = (g_externals_dir .. "fbxsdk/fbxsdk/lib/vs2013/")
    lib_name = "libfbxsdk.dll"
elseif (os.get() == "linux") then
    lib_dir = (g_externals_dir .. "fbxsdk/fbxsdk/lib/gcc4/")
    lib_name = "libfbxsdk.so"
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
configuration { "linux" }
    links { "fbxsdk" }
configuration {}

configuration { "x32", "Debug" }
    local fullLibPath = (lib_dir .. "x86/debug/" .. lib_name)
    postbuildcommands { path.translate("copy " .. fullLibPath .. " " .. g_output_dir) }
configuration { "x32", "Release" }
    local fullLibPath = (lib_dir .. "x86/release/" .. lib_name)
    postbuildcommands { path.translate("copy " .. fullLibPath .. " " .. g_output_dir) }
configuration { "x64", "Debug" }
    local fullLibPath = (lib_dir .. "x64/debug/" .. lib_name)
    postbuildcommands { path.translate("copy " .. fullLibPath .. " " .. g_output_dir) }
configuration { "x64", "Release" }
    local fullLibPath = (lib_dir .. "x64/release/" .. lib_name)
    postbuildcommands { path.translate("copy " .. fullLibPath .. " " .. g_output_dir) }
configuration {}
