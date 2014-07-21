
#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include "ShaderProgram.h"

class ShaderManager
{
public:
    ShaderManager() = default;
    ~ShaderManager();

    void CompileShaders(
        const std::vector<std::string> &vshader_files,
        const std::vector<std::string> &fshader_files,
        const std::vector<std::string> &ushader_files,
        std::vector<uint> &vertex_out,
        std::vector<uint> &fragment_out,
        std::vector<uint> &util_out);

    ShaderProgram CreateProgram();

private:
    auto CompileShaders(
        const std::vector<std::string> &shader_files, uint type)
            -> std::vector<uint>;

    auto CreateShaders(uint type, size_t num) -> std::vector<uint>;

private:
    std::unordered_set<uint> m_shaders;
    std::unordered_set<uint> m_programs;
    std::string m_codescratch;
};
