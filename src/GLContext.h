
#pragma once

#include <list>

#include "GL/glew.h"
#include "SDL_video.h"

#ifndef APIENTRY
#  ifdef _WIN32
#    if defined(__MINGW32__) || defined(__CYGWIN__)
#      define APIENTRY __stdcall
#    else (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#      define APIENTRY __stdcall
#    endif
#  endif // _WIN32
#  ifndef APIENTRY
#    define APIENTRY
#  endif // _APIENTRY
#endif // APIENTRY

class GLContext
{
public:
    GLContext(SDL_GLContext context);
    ~GLContext();

    auto SetDebugMessageCallback(GLDEBUGPROCARB fn,
                                 void* userdata = nullptr) -> bool;

    auto LoadShaders(const char *vertex_file,
                     const char *fragment_file) -> GLuint;

    void EnableDepthTest(GLenum func);

private:
    SDL_GLContext m_context;
    std::list<GLuint> m_programs;
};
