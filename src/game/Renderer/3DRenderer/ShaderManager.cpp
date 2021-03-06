
#include "ShaderManager.h"
#include "FileUtils.h"
#include "GL/glew.h"
#include "utils.h"
#include "glutils.h"

using namespace std;

ShaderManager::~ShaderManager()
{
    for (uint p : m_programs)
    {
        glDeleteProgram(p);
    }
    for (uint s : m_shaders)
    {
        glDeleteShader(s);
    }
}

void ShaderManager::CompileShaders(
    const vector<string> &vshader_files,
    const vector<string> &fshader_files,
    const vector<string> &uvert_shader_files,
    const vector<string> &ufrag_shader_files,
    vector<GLuint> &vertex_out,
    vector<GLuint> &fragment_out,
    vector<GLuint> &util_vert_out,
    vector<GLuint> &util_frag_out)
{
    vertex_out = CompileShaders(vshader_files, GL_VERTEX_SHADER);
    util_vert_out = CompileShaders(uvert_shader_files, GL_VERTEX_SHADER);
    fragment_out = CompileShaders(fshader_files, GL_FRAGMENT_SHADER);
    util_frag_out = CompileShaders(ufrag_shader_files, GL_FRAGMENT_SHADER);
}

ShaderProgram ShaderManager::CreateProgram()
{
    uint program = glCreateProgram();
    m_programs.insert(program);
    return ShaderProgram(program);
}

vector<GLuint> ShaderManager::CompileShaders(
    const vector<string> &shader_files, uint type)
{
    auto num_shaders = shader_files.size();
    auto shader_list = CreateShaders(type, num_shaders);

    int index = 0;
    for (auto &file : shader_files)
    {
        if (ReadBinaryFileToString(file, m_codescratch))
        {
            auto success = CompileShader(shader_list[index], m_codescratch);
            assert(success);
        }
        ++index;
    }
    return shader_list;
}

vector<GLuint> ShaderManager::CreateShaders(uint type, size_t num)
{
    vector<GLuint> shaders(num, 0);
    for (auto &s : shaders)
    {
        s = glCreateShader(type);
    }
    m_shaders.insert(shaders.cbegin(), shaders.cend());
    return shaders;
}
