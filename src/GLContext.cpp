
#include "GLContext.h"
#include "FileUtils.h"
#include "ShaderManager.h"

GLContext::GLContext(SDL_GLContext context):
    m_context(context)
{
}

GLContext::~GLContext()
{
    for (auto sm : m_shader_managers)
    {
        delete sm;
    }
}

ShaderManager *GLContext::CreateShaderManager()
{
    ShaderManager *sm = new ShaderManager();
    m_shader_managers.insert(sm);
    return sm;
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

void GLContext::EnableDepthTest(GLenum func)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(func);
}

void GLContext::SetActiveProgram(ShaderProgram *p)
{
    glUseProgram(p->m_id);
}
