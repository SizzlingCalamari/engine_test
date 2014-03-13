
#include "ShaderManager.h"
#include "FileUtils.h"
#include "GL/glew.h"
#include "utils.h"
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

ShaderProgram ShaderManager::CreateProgram()
{
    uint program = glCreateProgram();
    m_programs.insert(program);
    return ShaderProgram(program);
}

vector<Shader> ShaderManager::CompileShaders(
    const vector<string> &shader_files, uint type)
{
    size_t num_shaders = shader_files.size();
    vector<Shader> compiled = CreateShaders(type, num_shaders); 
    for (size_t i = 0; i < num_shaders; ++i)
    {
        FILE *ShaderFile = fopen(shader_files[i].c_str(), "rb");
        if (!ShaderFile)
        {
            continue;
        }

        off_t size = GetFileSize(ShaderFile);
        m_codescratch.resize(size+1);
        m_codescratch[size] = '\0';
        fread(&m_codescratch[0], 1, size, ShaderFile);
        fclose(ShaderFile);

        glShaderSource(compiled[i], 1, StrRValPtr(m_codescratch.data()), nullptr);
        glCompileShader(compiled[i]);

        int res = GL_FALSE;
        glGetShaderiv(compiled[i], GL_COMPILE_STATUS, &res);
        assert(res == GL_TRUE);
    }
    return compiled;
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
