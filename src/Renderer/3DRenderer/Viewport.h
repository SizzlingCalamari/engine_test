
#pragma once

#include "GL/glew.h"

class Viewport
{
public:
    Viewport(uint x = 0, uint y = 0, uint width = 0, uint height = 0):
        m_x(x),
        m_y(y),
        m_width(width),
        m_height(height)
    {
    }

    void SetView(uint x, uint y, uint width, uint height)
    {
        SetPosition(x, y);
        SetDimensions(width, height);
    }

    void SetPosition(uint x, uint y)
    {
        m_x = x;
        m_y = y;
    }

    void SetDimensions(uint width, uint height)
    {
        m_width = width;
        m_height = height;
    }

    void SetActive() const
    {
        glScissor(m_x, m_y, m_width, m_height);
        glViewport(m_x, m_y, m_width, m_height);
    }

    float GetAspectRatio() const
    {
        return static_cast<float>(m_width) / static_cast<float>(m_height);
    }

    bool operator==(const Viewport& other)
    {
        return m_all == other.m_all;
    }

    bool operator!=(const Viewport& other)
    {
        return !(*this == other);
    }

private:
    union
    {
        uint64 m_all;
        struct
        {
            uint16 m_x;
            uint16 m_y;
            uint16 m_width;
            uint16 m_height;
        };
    };
};
