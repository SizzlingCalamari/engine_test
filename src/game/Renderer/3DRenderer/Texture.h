
#pragma once

#include "GL/glew.h"

class Texture
{
public:
    void LoadTexture2D(const uint8* pixels, ptrdiff_t stride, uint width, uint height)
    {
        if (m_texture_2d == 0)
        {
            glGenTextures(1, &m_texture_2d);
        }
        assert(pixels);
        m_width = width;
        m_height = height;
        glBindTexture(GL_TEXTURE_2D, m_texture_2d);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        const auto format = (stride == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void FreeTexture()
    {
        if (m_texture_2d != 0)
        {
            glDeleteTextures(1, &m_texture_2d);
        }
    }

    uint GetWidth() const
    {
        return m_width;
    }

    uint GetHeight() const
    {
        return m_height;
    }

    GLuint GetGLId() const
    {
        return m_texture_2d;
    }

private:
    GLuint m_texture_2d = 0;
    uint m_width = 0;
    uint m_height = 0;
};
