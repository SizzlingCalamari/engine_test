
includedirs (g_externals_dir .. "glew/glew/include")
libdirs ("glew/lib")

configuration { "x64", "Debug" }
    links { "glew_" .. _ACTION .. "_x64_debug" }
configuration { "x64", "Release" }
    links { "glew_" .. _ACTION .. "_x64_release" }
configuration {}
