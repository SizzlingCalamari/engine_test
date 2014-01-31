
#include "GLContext.h"

GLContext::GLContext(SDL_GLContext context):
    m_context(context)
{
}

bool GLContext::SetDebugMessageCallback(
    GLDEBUGPROCARB fn, void* userdata /*= nullptr*/)
{
    if (GL_ARB_debug_output)
    {
        glDebugMessageCallbackARB(fn, userdata);
        glEnable(GL_DEBUG_OUTPUT);
        return true;
    }
    return false;
}

GLuint GLContext::LoadShaders(
    const char *vertex_file, const char *fragment_file)
{
    return 0;
}

void GLContext::EnableDepthTest(GLenum func)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(func);
}
