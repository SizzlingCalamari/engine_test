
#pragma once

#include <vector>
#include <string>
#include <unordered_set>

#include "GL/glew.h"

typedef uint Shader;
class ShaderProgram;

class ShaderManager
{
public:
    ShaderManager() = default;
    ~ShaderManager();

    void CompileShaders(
        const std::vector<std::string> &vshader_files,
        const std::vector<std::string> &fshader_files,
        std::vector<Shader> &vertex_out,
        std::vector<Shader> &fragment_out);

    auto CreateProgram() -> ShaderProgram;

private:
    auto CompileShaders(
        const std::vector<std::string> &shader_files, uint type)
            -> std::vector<Shader>;

    auto CreateShaders(uint type, uint num) -> std::vector<Shader>;

private:
    std::unordered_set<uint> m_shaders;
    std::unordered_set<uint> m_programs;
    std::string m_codescratch;
};

class ShaderProgram
{
    friend ShaderProgram ShaderManager::CreateProgram();
    friend class GLContext;

public:
    ShaderProgram(ShaderProgram&& other) = default;
    ~ShaderProgram() = default;

    void AttachShader(Shader s);
    bool Link();

    uint GetUniformLocation(const char *name);

protected:
    ShaderProgram(uint id);
    ShaderProgram& operator=(const ShaderProgram& other) = delete;

private:
    uint m_id;
};

inline ShaderProgram::ShaderProgram(uint id):
    m_id(id)
{
}

inline void ShaderProgram::AttachShader(Shader s)
{
    glAttachShader(m_id, s);
}

inline bool ShaderProgram::Link()
{
    glLinkProgram(m_id);
    int res = GL_FALSE;
    glGetProgramiv(m_id, GL_LINK_STATUS, &res);
    return res == GL_TRUE;
}

inline uint ShaderProgram::GetUniformLocation(const char *name)
{
    return glGetUniformLocation(m_id, name);
}
