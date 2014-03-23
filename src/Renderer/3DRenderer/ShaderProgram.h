
#pragma once

#include "GL/glew.h"
#include <string>
#include <unordered_map>

class ShaderProgram
{
public:
    ShaderProgram(GLuint id);

    void AttachShader(GLuint shader);

    bool Link(std::string* error_out = nullptr);

    void Bind();
    void Unbind();

    bool SetUniform(const char* name, void* data);

private:
    GLuint m_id;

    struct UniformInfo
    {
        GLuint index;
        GLenum type;
    };

    std::unordered_map<std::string, UniformInfo> m_uniforms;
};
