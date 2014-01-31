
#pragma once

#include "GL/glew.h"
#include "SDL.h"

#ifndef APIENTRY
    #define APIENTRY GLAPIENTRY
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
