
includedirs (g_externals_dir .. "bulletphysics/bullet3/src")
libdirs ("bullet3/lib")

configuration { "x32", "Debug" }
    suffix = _ACTION .. "_debug"
    links {
        "BulletSoftBody_" .. suffix,
        "BulletDynamics_" .. suffix,
        "BulletCollision_" .. suffix,
        "LinearMath_" .. suffix
    }
configuration { "x32", "Release" }
    suffix = _ACTION
    links {
        "BulletSoftBody_" .. suffix,
        "BulletDynamics_" .. suffix,
        "BulletCollision_" .. suffix,
        "LinearMath_" .. suffix
    }
configuration { "x64", "Debug" }
    suffix = _ACTION .. "_x64_debug"
    links {
        "BulletSoftBody_" .. suffix,
        "BulletDynamics_" .. suffix,
        "BulletCollision_" .. suffix,
        "LinearMath_" .. suffix
    }
configuration { "x64", "Release" }
    suffix = _ACTION .. "_x64_release"
    links {
        "BulletSoftBody_" .. suffix,
        "BulletDynamics_" .. suffix,
        "BulletCollision_" .. suffix,
        "LinearMath_" .. suffix
    }
configuration {}
