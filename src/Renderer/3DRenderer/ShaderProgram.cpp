
#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(GLuint id):
    m_id(id)
{
}

void ShaderProgram::AttachShader(GLuint shader)
{
    glAttachShader(m_id, shader);
}

bool ShaderProgram::Link(std::string* error_out /*= nullptr*/)
{
    // link the program
    glLinkProgram(m_id);

    // check if the link was successful
    GLint status = GL_FALSE;
    glGetProgramiv(m_id, GL_LINK_STATUS, &status);

    // if it linked, find and cache the uniform names and ids
    if (GL_TRUE == status)
    {
        // get the number of active uniforms
        GLint num_uniforms = 0;
        glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &num_uniforms);

        // get the length of the largest active uniform name
        GLint name_max_len = 0;
        glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_max_len);

        std::string name;
        for (GLint i = 0; i < num_uniforms; ++i)
        {
            name.resize(name_max_len);
            GLsizei length = 0;
            GLint size = 0;
            GLenum type = 0;
            // length == 0 means error
            // size > 1 means the type is an array
            glGetActiveUniform(m_id, i, name_max_len, &length, &size, &type, &name.front());
            if (length != 0)
            {
                // resize the string to the real name length
                name.resize(length);

                // insert the uniform info into the hash map
                m_uniforms.emplace(std::move(name), UniformInfo{ i, type });
            }
        }
    }
    else if (error_out && (GL_FALSE == status))
    {
        // write out the error to the output string
        GLint log_length = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &log_length);

        error_out->resize(log_length);
        glGetProgramInfoLog(m_id, log_length, nullptr, &error_out->front());
    }

    return (GL_TRUE == status);
}

void ShaderProgram::Bind()
{
    glUseProgram(m_id);
}

void ShaderProgram::Unbind()
{
    glUseProgram(0);
}

bool ShaderProgram::SetUniform(const char* name, void* data)
{
    // check if the uniform name exists in the program
    auto it = m_uniforms.find(name);
    if (it == m_uniforms.end())
    {
        return false;
    }

    // cast the data and call the correct uniform method
    UniformInfo info = it->second;
    switch (info.type)
    {
    case GL_FLOAT_MAT4:
    {
        float* data_float = static_cast<float*>(data);
        glUniformMatrix4fv(info.index, 1, GL_FALSE, data_float);
    }
        break;
    default:
        return false;
    }
    return true;
}