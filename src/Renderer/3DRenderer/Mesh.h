
#pragma once

#include "GL/glew.h"

class Mesh
{
public:
    void LoadVerticies(const void* verticies, uint stride, uint num_verticies)
    {
        if (m_vertex_buffer == 0)
        {
            glGenBuffers(1, &m_vertex_buffer);
        }
        assert(verticies);
        m_num_verticies = num_verticies;
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, num_verticies*stride, verticies, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void FreeVerticies()
    {
        if (m_vertex_buffer != 0)
        {
            glDeleteBuffers(1, &m_vertex_buffer);
        }
    }

    uint GetNumVerticies() const
    {
        return m_num_verticies;
    }

    GLuint GetVertexBufferId() const
    {
        return m_vertex_buffer;
    }

private:
    GLuint m_vertex_buffer = 0;
    uint m_num_verticies = 0;
};
