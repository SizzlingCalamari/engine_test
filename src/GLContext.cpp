
#include "GLContext.h"
#include "FileUtils.h"

GLContext::GLContext(SDL_GLContext context):
    m_context(context)
{
}

GLContext::~GLContext()
{
    for (GLuint program : m_programs)
    {
        glDeleteProgram(program);
    }
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
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    FILE *VertexShaderFile = fopen(vertex_file, "rb");
    FILE *FragmentShaderFile = fopen(fragment_file, "rb");

    if (!VertexShaderFile || !FragmentShaderFile)
    {
        if (VertexShaderFile) fclose(VertexShaderFile);
        if (FragmentShaderFile) fclose(FragmentShaderFile);
        return 0;
    }

    off_t vertex_size = GetFileSize(VertexShaderFile);
    off_t fragment_size = GetFileSize(FragmentShaderFile);

    char *VertexShaderCode = new char[vertex_size + 1];
    char *FragmentShaderCode = new char[fragment_size + 1];

    VertexShaderCode[vertex_size] = '\0';
    FragmentShaderCode[fragment_size] = '\0';
    
    fread(VertexShaderCode, 1, vertex_size, VertexShaderFile);
    fread(FragmentShaderCode, 1, fragment_size, FragmentShaderFile);

    fclose(VertexShaderFile);
    fclose(FragmentShaderFile);

    glShaderSource(VertexShaderID, 1, (const char**)&VertexShaderCode, nullptr);
    glCompileShader(VertexShaderID);

    glShaderSource(FragmentShaderID, 1, (const char**)&FragmentShaderCode, nullptr);
    glCompileShader(FragmentShaderID);

    GLint res = GL_FALSE;

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &res);
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &res);

    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &res);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    delete [] FragmentShaderCode;
    delete [] VertexShaderCode;

    m_programs.emplace_back(ProgramID);
    return ProgramID;
}

void GLContext::EnableDepthTest(GLenum func)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(func);
}
