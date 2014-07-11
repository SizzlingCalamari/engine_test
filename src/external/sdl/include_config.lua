
local copy_cmd = "copy " .. g_externals_dir .. "sdl/SDL/VisualC/SDL/"
local copy_dest = g_output_dir .. "SDL2.dll"
local full_cmd = ""

includedirs (g_externals_dir .. "sdl/SDL/include")

configuration { "windows" }
    libdirs ("SDL/VisualC/SDL")
configuration { "linux" }
    libdirs { "SDL/build/", "SDL/build/.libs" }

local lib_name = ""
configuration { "windows", "x32", "Debug" }
    lib_name = "/Win32/Debug/SDL2"
    full_cmd = path.translate(copy_cmd .. "Win32/Debug/SDL2.dll " .. copy_dest)
    postbuildcommands { full_cmd }
configuration { "windows", "x32", "Release" }
    lib_name = "/Win32/Release/SDL2"
    full_cmd = path.translate(copy_cmd .. "Win32/Release/SDL2.dll " .. copy_dest)
    postbuildcommands { full_cmd }
configuration { "windows", "x64", "Debug" }
    lib_name = "/x64/Debug/SDL2"
    full_cmd = path.translate(copy_cmd .. "x64/Debug/SDL2.dll " .. copy_dest)
    postbuildcommands { full_cmd }
configuration { "windows", "x64", "Release" }
    lib_name = "/x64/Release/SDL2"
configuration { "windows" }
    links { lib_name }
    full_cmd = path.translate(copy_cmd .. lib_name .. ".dll " .. copy_dest)
    postbuildcommands { full_cmd }
configuration { "linux" }
    links { "SDL2" }
-- TODO: osx sdl linking
configuration {}
