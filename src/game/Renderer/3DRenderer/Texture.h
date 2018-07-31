
#pragma once

#include <glm/vec3.hpp>
#include "GL/glew.h"

class Texture
{
public:
    Texture();
    Texture(const Texture& other) = delete;
    Texture(Texture&& other);
    Texture& operator=(Texture&& other);
    Texture& operator=(const Texture& other) = delete;
    ~Texture();

    void LoadTexture2D(const uint8* pixels, std::ptrdiff_t stride, uint width, uint height);

    void FreeTexture();

    uint GetWidth() const;

    uint GetHeight() const;

    uint GetGLId() const;

private:
    uint m_texture_2d;
    uint m_width;
    uint m_height;
};

inline uint Texture::GetWidth() const
{
    return m_width;
}

inline uint Texture::GetHeight() const
{
    return m_height;
}

inline uint Texture::GetGLId() const
{
    return m_texture_2d;
}

class Texture3D
{
public:
    Texture3D():
        m_texture(0),
        m_dimensions(0)
    {
    }

    void LoadTexture3D(const uint8* texels,
                       GLenum format, GLenum type,
                       uint width, uint height, uint depth);

    void FreeTexture();

    const glm::uvec3& GetDimensions() const
    {
        return m_dimensions;
    }

    uint GetGLId() const
    {
        return m_texture;
    }
    
private:
    uint m_texture;
    glm::uvec3 m_dimensions;
};
