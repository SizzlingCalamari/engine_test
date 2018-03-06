
local lib_dir = ""
local lib_name = ""
local path_sep = ""
if (os.get() == "windows") then
    lib_dir = (g_externals_dir .. "sdl/SDL/VisualC/SDL/")
    lib_name = "SDL2.dll"
    path_sep = "\\"
elseif (os.get() == "linux") then
    lib_dir = (g_externals_dir .. "sdl/SDL/build/")
    lib_name = "libSDL2*.so*"
    path_sep = "/"
elseif (os.get() == "macosx") then
    lib_dir = (g_externals_dir .. "sdl/SDL/build/")
    lib_name = "libSDL2*.dylib*"
    path_sep = "/"
end

includedirs (g_externals_dir .. "sdl/SDL/include")

configuration { "windows", "x32", "Debug" }
    libdirs (lib_dir .. "Win32/debug")
configuration { "windows", "x32", "Release" }
    libdirs (lib_dir .. "Win32/release")
configuration { "windows", "x64", "Debug" }
    libdirs (lib_dir .. "x64/debug")
configuration { "windows", "x64", "Release" }
    libdirs (lib_dir .. "x64/release")
configuration { "linux or macosx" }
    libdirs { lib_dir, (lib_dir .. ".libs/") }
configuration {}

links "SDL2"

configuration { "windows", "x32", "Debug" }
    local fullLibPath = (lib_dir .. "Win32/debug/" .. lib_name)
    postbuildcommands { path.translate("copy " .. fullLibPath .. " " .. g_output_dir, path_sep) }
configuration { "windows", "x32", "Release" }
    local fullLibPath = (lib_dir .. "Win32/release/" .. lib_name)
    postbuildcommands { path.translate("copy " .. fullLibPath .. " " .. g_output_dir, path_sep) }
configuration { "windows", "x64", "Debug" }
    local fullLibPath = (lib_dir .. "x64/debug/" .. lib_name)
    postbuildcommands { path.translate("copy " .. fullLibPath .. " " .. g_output_dir, path_sep) }
configuration { "windows", "x64", "Release" }
    local fullLibPath = (lib_dir .. "x64/release/" .. lib_name)
    postbuildcommands { path.translate("copy " .. fullLibPath .. " " .. g_output_dir, path_sep) }
configuration { "linux or macosx" }
    local fullLibPath = (lib_dir .. ".libs/" .. lib_name)
    postbuildcommands { path.translate("cp " .. fullLibPath .. " " .. g_output_dir, path_sep) }
configuration {}
