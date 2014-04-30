
local sdl_vc_dir = g_externals_dir .. "sdl/SDL/VisualC/SDL/"

local copy_cmd = "copy " .. sdl_vc_dir
local copy_dest = g_output_dir .. "SDL2.dll"
local full_cmd = ""

includedirs (g_externals_dir .. "sdl/SDL/include")
libdirs ("SDL/VisualC/SDL")

configuration { "windows", "x32", "Debug" }
    links { "/Win32/Debug/SDL2" }
    full_cmd = path.translate(copy_cmd .. "Win32/Debug/SDL2.dll " .. copy_dest)
    postbuildcommands { full_cmd }
configuration { "windows", "x32", "Release" }
    links { "/Win32/Release/SDL2" }
    full_cmd = path.translate(copy_cmd .. "Win32/Release/SDL2.dll " .. copy_dest)
    postbuildcommands { full_cmd }
configuration { "windows", "x64", "Debug" }
    links { "/x64/Debug/SDL2" }
    full_cmd = path.translate(copy_cmd .. "x64/Debug/SDL2.dll " .. copy_dest)
    postbuildcommands { full_cmd }
configuration { "windows", "x64", "Release" }
    links { "/x64/Release/SDL2" }
    full_cmd = path.translate(copy_cmd .. "x64/Release/SDL2.dll " .. copy_dest)
    postbuildcommands { full_cmd }
configuration { "linux" }
    links { "SDL2" }
-- TODO: osx sdl linking
configuration {}
