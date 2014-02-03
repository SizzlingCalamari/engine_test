
#pragma once

#include "GL/glew.h"
#include "SDL.h"
//#include "SDL_opengl.h"

#ifdef _WIN32
#ifndef APIENTRY
#  if defined(__MINGW32__) || defined(__CYGWIN__)
#    define APIENTRY __stdcall
#  elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#    define APIENTRY __stdcall
#  else
#    define APIENTRY
#  endif
#endif
#endif

class GLContext
{
public:
    GLContext(SDL_GLContext context);
    ~GLContext() = default;

    auto SetDebugMessageCallback(
        GLDEBUGPROCARB fn, void* userdata = nullptr)
            -> bool;

    auto LoadShaders(
        const char *vertex_file, const char *fragment_file)
            -> GLuint;

    auto EnableDepthTest(GLenum func) -> void;

private:
    SDL_GLContext m_context;
};
