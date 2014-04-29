
#pragma once

#include <unordered_set>

#include "GL/glew.h"

class ShaderManager;

class GLContext
{
public:
    GLContext(void *context);
    ~GLContext();

    auto CreateShaderManager() -> ShaderManager*;

    static bool SetDebugMessageCallback(GLDEBUGPROCARB fn,
                                        void* userdata = nullptr);

    static void EnableDepthTest(GLenum func);

private:
    void *m_context;
    std::unordered_set<ShaderManager*> m_shader_managers;
};
