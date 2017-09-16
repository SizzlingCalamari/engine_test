
#include "Application/Application.h"
#include "Engine.h"

#include <pmmintrin.h>
#include <xmmintrin.h>
#include <memory>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

// gcc < 5.0 doesn't have std::align
// Copied from trunk:
#if defined(__GNUC__) && (__GNUC__ < 5)

#include <cstdint>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

inline void* align(size_t __align, size_t __size, void*& __ptr, size_t& __space) noexcept
{
    const auto __intptr = reinterpret_cast<uintptr_t>(__ptr);
    const auto __aligned = (__intptr - 1u + __align) & -__align;
    const auto __diff = __aligned - __intptr;
    if ((__size + __diff) > __space)
    {
        return nullptr;
    }
    else
    {
        __space -= __diff;
        return __ptr = reinterpret_cast<void*>(__aligned);
    }
}

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif

template<typename T, size_t Align>
class heap_aligned_alloc
{
public:
    heap_aligned_alloc()
    {
        std::size_t length = sizeof(T) + Align - 1;
        void* ptr = malloc(length);
        m_mem = reinterpret_cast<unsigned char*>(ptr);
        std::align(Align, sizeof(T), ptr, length);
        m_ptr = new(ptr) T;
    }

    ~heap_aligned_alloc()
    {
        m_ptr->~T();
        free(m_mem);
        m_ptr = nullptr;
        m_mem = nullptr;
    }

    inline T* operator->() const
    {
        return m_ptr;
    }

private:
    T* m_ptr;
    unsigned char* m_mem;
};

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

    {
        heap_aligned_alloc<Engine, 16U> engine;

        engine->Initialize();
        engine->Run();
        engine->Shutdown();
    }

    ApplicationService::Shutdown();
    return 0;
}
