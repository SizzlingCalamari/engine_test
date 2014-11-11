
#include <iostream>
#include "Application/Application.h"

#include "Engine.h"
#include <type_traits>

using EngineStorage = std::aligned_storage<sizeof(Engine), 16U>::type;

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <pmmintrin.h>
#include <xmmintrin.h>

int main(int argc, const char *argv[])
{
    {
        // disable denormal floats
        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
        _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
    }
    {
        // fix up working directory
        char temp[128] = {};
        const char *dir = getcwd(temp, sizeof(temp));
        const char *bin_pos = strstr(dir, "bin");
        const char *build_pos = strstr(dir, "build");
        chdir("..");
        if (build_pos > bin_pos)
        {
            chdir("..");
        }
    }
    ApplicationService::Initialize();

    auto *engine_storage = new EngineStorage;

    Engine *engine = new(engine_storage) Engine;
    engine->Initialize();
    engine->Run();
    engine->Shutdown();
    engine->~Engine();

    delete engine_storage;

    ApplicationService::Shutdown();
    return 0;
}
