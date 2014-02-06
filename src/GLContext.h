
#pragma once

#include <list>

#include "GL/glew.h"
//#include "SDL_video.h"
typedef void *SDL_GLContext;

class GLContext
{
public:
    GLContext(SDL_GLContext context);
    ~GLContext();

    auto SetDebugMessageCallback(GLDEBUGPROCARB fn,
                                 void* userdata = nullptr) -> bool;

    auto LoadShaders(const char *vertex_file,
                     const char *fragment_file) -> uint;

    void EnableDepthTest(GLenum func);

private:
    SDL_GLContext m_context;
    std::list<uint> m_programs;
};
