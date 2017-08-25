
#include "Texture.h"
#include "GL/glew.h"

void Texture::LoadTexture2D(const uint8* pixels, ptrdiff_t stride, uint width, uint height)
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

    float maxAniso;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    const auto format = (stride == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::FreeTexture()
{
    if (m_texture_2d != 0)
    {
        glDeleteTextures(1, &m_texture_2d);
    }
}

void Texture3D::LoadTexture3D(const uint8* texels,
                              GLenum format, GLenum type,
                              uint width, uint height, uint depth)
{
    if (m_texture == 0)
    {
        glGenTextures(1, &m_texture);
    }
    assert(texels);
    m_dimensions = glm::vec3(width, height, depth);

    glBindTexture(GL_TEXTURE_3D, m_texture);

    // linear filtering
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // repeated texture
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, format, type, texels);

    glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::FreeTexture()
{
    if (m_texture != 0)
    {
        glDeleteTextures(1, &m_texture);
    }
}
