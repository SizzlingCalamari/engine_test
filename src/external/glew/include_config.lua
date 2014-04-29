
includedirs (g_externals_dir .. "glew/glew/include")
libdirs ("glew/lib")

configuration { "x32", "Debug" }
    links { "glew_" .. _ACTION .. "_x86_debug" }
configuration { "x32", "Release" }
    links { "glew_" .. _ACTION .. "_x86_release" }
configuration { "x64", "Debug" }
    links { "glew_" .. _ACTION .. "_x64_debug" }
configuration { "x64", "Release" }
    links { "glew_" .. _ACTION .. "_x64_release" }
configuration {}
