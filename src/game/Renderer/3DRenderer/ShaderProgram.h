
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

    bool SetUniform(const char* name, const void* data);

private:
    void QueryUniformInformation();
    void QueryAttributesInformation();

private:
    GLuint m_id;

    // Used for enabling only the needed
    // vertex attribute indicies when rendering.
    // Test performance of this vs enabling all (16)
    // each frame.
    GLint m_num_attributes;

    struct UniformInfo
    {
        GLuint index;
        GLenum type;
    };
    std::unordered_map<std::string, UniformInfo> m_uniforms;
};
