
#pragma once

#include "GL/glew.h"
#include <string>
#include <iostream>

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
            if (res == GL_FALSE)
            {
                GLint length;
                glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
                std::string log(length, '\0');
                glGetShaderInfoLog(shader_id, length, nullptr, &log[0]);
                std::cout << log << std::endl;
            }
            return (GL_TRUE == res);
        }
        return true;
    }

    inline bool LinkProgram(uint programId)
    {
        // link the program
        glLinkProgram(programId);

        // check if the link was successful
        GLint status = GL_FALSE;
        glGetProgramiv(programId, GL_LINK_STATUS, &status);

        if (GL_FALSE == status)
        {
            // write out the error to the output string
            GLint log_length = 0;
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &log_length);

            std::string log(log_length, '\0');
            glGetProgramInfoLog(programId, log_length, nullptr, &log[0]);
            std::cout << log << std::endl;
        }

        return (GL_TRUE == status);
    }
}
