
#pragma once

#include <string>
#include "GL/glew.h"

namespace
{
    inline bool CompileShader(uint shader_id, const std::string &shader_source, bool check_error = true)
    {
        auto length = shader_source.length();
        auto source = shader_source.c_str();

        glShaderSource(shader_id, 1, &source, (const int*)&length);
        glCompileShader(shader_id);

        if (check_error)
        {
            auto res = GL_FALSE;
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
            return (GL_TRUE == res);
        }
        return true;
    }
}
