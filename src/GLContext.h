
#pragma once

#include <unordered_set>

#include "GL/glew.h"
//#include "SDL_video.h"
typedef void *SDL_GLContext;
class ShaderManager;
class ShaderProgram;

class GLContext
{
public:
    GLContext(SDL_GLContext context);
    ~GLContext();

    auto CreateShaderManager() -> ShaderManager*;

    static bool SetDebugMessageCallback(GLDEBUGPROCARB fn,
                                        void* userdata = nullptr);

    static void EnableDepthTest(GLenum func);

    static void SetActiveProgram(ShaderProgram *p);

private:
    SDL_GLContext m_context;
    std::unordered_set<ShaderManager*> m_shader_managers;
};
