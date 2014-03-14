
#pragma once

#include "GL/glew.h"

class Model
{
public:
    Model():
        m_num_verticies(0)
    {
        glGenBuffers(1, &m_vertex_buffer);
    }

    ~Model()
    {
        glDeleteBuffers(1, &m_vertex_buffer);
    }

    void LoadVerticies(const float* verticies, uint num_verticies)
    {
        assert(verticies);
        m_num_verticies = num_verticies;
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, num_verticies*3*sizeof(float), verticies, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    uint GetNumVerticies() const
    {
        return m_num_verticies;
    }

    uint GetVertexBufferId() const
    {
        return m_vertex_buffer;
    }

private:
    uint m_vertex_buffer;
    uint m_num_verticies;
};
