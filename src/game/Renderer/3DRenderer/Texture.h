
#pragma once

class Texture
{
public:
    void LoadTexture2D(const uint8* pixels, std::ptrdiff_t stride, uint width, uint height);

    void FreeTexture();

    uint GetWidth() const;

    uint GetHeight() const;

    uint GetGLId() const;

private:
    uint m_texture_2d = 0;
    uint m_width = 0;
    uint m_height = 0;
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
