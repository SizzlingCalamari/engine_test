
#include "ShaderProgram.h"
#include "glutils.h"

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
    bool success = LinkProgram(m_id);

    // if it linked, find and cache the uniform names and ids
    if (success)
    {
        // lookup and store uniform information
        QueryUniformInformation();

        // store the number of active attributes
        // for activating before using the shader
        glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &m_num_attributes);
    }

    return success;
}

void ShaderProgram::Bind()
{
    glUseProgram(m_id);
    for (GLint i = 0; i < m_num_attributes; ++i)
    {
        //glEnableVertexAttribArray(i);
    }
}

void ShaderProgram::Unbind()
{
    for (GLint i = 0; i < m_num_attributes; ++i)
    {
        glDisableVertexAttribArray(i);
    }
    glUseProgram(0);
}

bool ShaderProgram::SetUniform(const std::string& name, const void* data)
{
    return SetUniform(name.c_str(), data);
}

bool ShaderProgram::SetUniform(const char* name, const void* data)
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
    case GL_FLOAT_MAT3x2:
        {
            const GLfloat* mat = static_cast<const GLfloat*>(data);
            glUniformMatrix3x2fv(info.index, 1, GL_FALSE, mat);
        }
        break;
    case GL_FLOAT_MAT4:
        {
            auto mat4 = static_cast<const float*>(data);
            glUniformMatrix4fv(info.index, 1, GL_FALSE, mat4);
        }
        break;
    case GL_SAMPLER_2D:
        {
            int value = *static_cast<const int*>(data);
            glUniform1i(info.index, value);
        }
        break;
    case GL_FLOAT_VEC3:
        {
            auto float3 = static_cast<const float*>(data);
            glUniform3f(info.index, *float3, *(float3 + 1), *(float3 + 2));
        }
        break;
    case GL_FLOAT:
        {
            float value = *static_cast<const float*>(data);
            glUniform1f(info.index, value);
        }
        break;
    case GL_INT:
        {
            int value = *static_cast<const int*>(data);
            glUniform1i(info.index, value);
        }
        break;
    case GL_BOOL:
        {
            bool value = *static_cast<const bool*>(data);
            glUniform1i(info.index, value);
        }
        break;
    default:
        return false;
    }
    return true;
}

void ShaderProgram::QueryUniformInformation()
{
    // get the number of active uniforms
    GLint num_uniforms = 0;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &num_uniforms);

    // get the length of the largest active uniform name
    GLint name_max_len = 0;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_max_len);

    // query the uniform names for fast lookups
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

            const GLint location = glGetUniformLocation(m_id, name.c_str());
            if(location != -1)
            {
                // insert the uniform info into the hash map
                m_uniforms.emplace(std::move(name), UniformInfo{ static_cast<GLuint>(location), type });
            }
        }
    }
}

void ShaderProgram::QueryAttributesInformation()
{
    // get the number of active attributes
    glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &m_num_attributes);
}
