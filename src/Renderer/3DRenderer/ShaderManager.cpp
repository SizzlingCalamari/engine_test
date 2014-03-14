
#include "ShaderManager.h"
#include "FileUtils.h"
#include "GL/glew.h"
#include "utils.h"
#include "glutils.h"
#include <cassert>

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
    vector<Shader> &vertex_out,
    vector<Shader> &fragment_out)
{
    vertex_out = CompileShaders(vshader_files, GL_VERTEX_SHADER);
    fragment_out = CompileShaders(fshader_files, GL_FRAGMENT_SHADER);
}

uint ShaderManager::CreateProgram()
{
    uint program = glCreateProgram();
    m_programs.insert(program);
    return program;
}

vector<Shader> ShaderManager::CompileShaders(
    const vector<string> &shader_files, uint type)
{
    int num_shaders = shader_files.size();
    auto shader_list = CreateShaders(type, num_shaders);

    for (auto i = 0; i < num_shaders; ++i)
    {
        if (ReadBinaryFileToString(shader_files[i], m_codescratch))
        {
            auto success = CompileShader(shader_list[i], m_codescratch);
            assert(success);
        }
    }
    return shader_list;
}

vector<Shader> ShaderManager::CreateShaders(uint type, uint num)
{
    vector<Shader> shaders(num, 0);
    for (auto &s : shaders)
    {
        s = glCreateShader(type);
    }
    m_shaders.insert(shaders.cbegin(), shaders.cend());
    return shaders;
}
