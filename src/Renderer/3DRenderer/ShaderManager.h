
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

    uint CreateProgram();

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
