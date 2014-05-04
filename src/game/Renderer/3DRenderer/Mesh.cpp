
#include "Mesh.h"
#include "GL/glew.h"

void Mesh::LoadVerticies(const void* verticies, ptrdiff_t stride, size_t num_verticies)
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

void Mesh::FreeVerticies()
{
    if (m_vertex_buffer != 0)
    {
        glDeleteBuffers(1, &m_vertex_buffer);
    }
}
